#include <cross_cut/logger.h>
#include <mapping_core/map_actor.h>
#include <mapping_core/map_image.h>
#include "map_animations.h"

extern Logger logger;

u32 iscriptIdFromImage(const Sc::Data & scData, u32 imageId)
{
    return scData.sprites.getImage(imageId).iScriptId;
}

void MapImage::error(std::string_view message)
{
    logger.error(message);
    end();
}

bool MapImage::end()
{
    waitUntil = std::numeric_limits<uint64_t>::max();
    return true;
}

bool MapImage::hasEnded() const
{
    return waitUntil == std::numeric_limits<uint64_t>::max();
}

void MapImage::playFrame(std::size_t frame)
{
    if ( this->baseFrame != frame )
    {
        this->baseFrame = frame;
        this->frame = this->baseFrame + std::size_t(this->direction);
    }
}

void MapImage::setDirection(u8 direction)
{
    if ( rotation )
    {
        this->direction = (direction + 4) / 8;
        this->flipped = this->direction > 16;
        if ( this->flipped )
            this->direction = 32 - this->direction;

        this->frame = this->baseFrame + this->direction;
    }
}

void MapImage::advanceBy(size_t numBytes)
{
    ((u8* &)animation) += numBytes;
}

void Animator::setActorDirection(u8 direction)
{
    context.actor.direction = direction;
    for ( std::ptrdiff_t i=std::size(context.actor.usedImages)-1; i>=0 ; --i )
    {
        if ( context.actor.usedImages[i] != 0 )
            context.animations.images[context.actor.usedImages[i]]->setDirection(context.actor.direction);
    }
}

void Animator::initializeImage(std::size_t iScriptId)
{
    const auto & scData = context.animations.scData;
    auto & imageDat = scData.sprites.getImage(currImage->imageId);

    currImage->remapping = imageDat.remapping;
    if ( currImage->remapping != 0 )
        currImage->drawFunction = MapImage::DrawFunction::Remap;

    currImage->drawIfCloaked = imageDat.drawIfCloaked != 0;
    currImage->iScriptId = iScriptId;
    currImage->animation = scData.sprites.getAnimationHeader(iScriptId, Sc::Sprite::AnimHeader::StarEditInit);
    if ( currImage->animation == nullptr )
        currImage->animation = scData.sprites.getAnimationHeader(iScriptId, Sc::Sprite::AnimHeader::Init);

    currImage->rotation = scData.sprites.getImage(currImage->imageId).graphicTurns != 0;
    if ( currImage->animation == nullptr )
        currImage->end();
    else
    {
        currImage->waitUntil = context.currentTick;
        animate(); // Advance until the first wait such that the image starts on the correct frame and all
        currImage->setDirection(context.actor.direction);
        const Sc::Sprite::IScriptAnimation* built = scData.sprites.getAnimationHeader(iScriptId, Sc::Sprite::AnimHeader::Built);
        if ( built )
        {
            currImage->waitUntil = context.currentTick;
            currImage->animation = built;
            currImage->returnOffset = 0;
            animate();
        }
    }
}

void Animator::endImage()
{
    if ( currImageIndex == context.actor.usedImages[context.actor.primaryImageSlot] )
        currImage->end();
    else
    {
        context.actor.removeImage(currImageIndex);
        context.animations.removeImage(currImageIndex);
    }
}

void Animator::createOverlay(u16 imageId, s8 x, s8 y, bool above)
{
    MapImage* primaryImage = context.actor.primaryImage(context.animations);
    if ( primaryImage == nullptr )
    {
        logger.warn("CreateOverlay called but there was no primary image for the actor.");
        return;
    }
    auto & overlayImageIndex = context.actor.getNewImageSlot(above, *currImage, context.animations);
    if ( overlayImageIndex != 0 )
    {
        logger.warn("CreateOverlay called but no image slots were available for the actor.");
        return;
    }
    const auto & scData = context.animations.scData;
    overlayImageIndex = context.animations.createImage();
    this->currImage = &context.animations.images[this->currImageIndex].value(); // Ensure currImage still valid if images was resized
    primaryImage = context.actor.primaryImage(context.animations); // Ensure primaryImage still valid if images was resized
    //logger.info() << "PrimaryImage: " << usedImages[0] << ", " << primaryImage.xc << ", " << primaryImage.yc << '\n';
    auto & overlayImage = context.animations.images[overlayImageIndex].value();
    overlayImage.rotation = scData.sprites.getImage(imageId).graphicTurns != 0;
    overlayImage.imageId = imageId;
    overlayImage.owner = primaryImage->owner;
    overlayImage.xc = primaryImage->xc + s32(x);
    overlayImage.yc = primaryImage->yc + s32(y);
    overlayImage.remapping = scData.sprites.getImage(imageId).remapping;
    overlayImage.drawFunction = overlayImage.remapping != 0 ? MapImage::DrawFunction::Remap : MapImage::DrawFunction::Normal;
    overlayImage.drawFunction = (MapImage::DrawFunction)scData.sprites.getImage(imageId).drawFunction;
    //logger.info() << this << " creating overlay: " << &overlayImage << '\n';
    Animator {
        .context = this->context,
        .currImageIndex = overlayImageIndex,
        .currImage = &overlayImage
    }.initializeImage(iscriptIdFromImage(scData, imageId));
    //logger.info() << "CreateOverlay: " << overlayImageIndex << ", " << imageId << ", " << "(" << overlayImage.xc << ", " << overlayImage.yc << ")\n";
}

void Animator::createSpriteOverlay(u16 spriteId, s8 x, s8 y, bool above)
{
    u16 imageId = context.animations.scData.sprites.getSprite(spriteId).imageFile;
    //logger.info() << "createSpriteOverlay " << imageId << ", " << x << ", " << y << '\n';
    createOverlay(imageId, x, y, above);
}

void Animator::animate()
{
    const auto & scData = context.animations.scData;
    auto & images = context.animations.images;
    auto & iscript = scData.sprites.iscript;
    size_t currOffset = std::distance((const u8*)&iscript[0], (const u8*)currImage->animation);
    if ( context.currentTick >= currImage->waitUntil )
    {
        // Step into the next or couple next iscript ops
        for ( ; ; )
        {
            if ( currOffset >= iscript.size() )
            {
                logger.info() << "  " << currImage << '\n';
                return currImage->error("Unknown IScript Overflow");
            }

            Sc::Sprite::Op code = Sc::Sprite::Op(currImage->animation->code);
            ++currOffset; // 1-byte code
            std::string opName = code < Sc::Sprite::OpName.size() ? std::string(Sc::Sprite::OpName[code]) : std::to_string(int(code));
            //if ( context.actor.usedImages[context.actor.primaryImageSlot] == currImageIndex )
            //    logger.info() << "  " << this->currImage->iScriptId << ", " << this->currImage->imageId << ", " << opName << ", \n";

            if ( code < Sc::Sprite::OpParams.size() )
            {
                auto & opCodeParams = Sc::Sprite::OpParams[code];

                auto advancePastParams = [&](){
                    // Move currOffset past all the parameters
                    for ( int param=0; param<opCodeParams.size(); ++param )
                    {
                        auto currParam = opCodeParams[param];
                        auto paramSize = Sc::Sprite::ParamSize[size_t(currParam)];
                        u16 paramValue = paramSize == 1 ? iscript[currOffset] : (u16 &)iscript[currOffset];
                        currOffset += paramSize;
                    }
                    if ( currOffset < iscript.size() )
                        currImage->animation = (Sc::Sprite::IScriptAnimation*)&iscript[currOffset];
                };
                switch ( code )
                {
                    case Sc::Sprite::Op::playfram:
                        currImage->playFrame((u16 &)iscript[currOffset]);
                        break;
                    case Sc::Sprite::Op::playframtile: // TODO
                        break;
                    case Sc::Sprite::Op::sethorpos:
                        currImage->xOffset = s8(iscript[currOffset]);
                        break;
                    case Sc::Sprite::Op::setvertpos:
                        currImage->yOffset = s8(iscript[currOffset]);
                        break;
                    case Sc::Sprite::Op::setpos:
                        currImage->xOffset = s8(iscript[currOffset]);
                        currImage->yOffset = s8(iscript[currOffset+1]);
                        break;
                    case Sc::Sprite::Op::wait:
                        if ( unbreak )
                        {
                            advancePastParams();
                            return;
                        }
                        currImage->waitUntil = context.currentTick + std::uint64_t(iscript[currOffset]);
                        if ( Sc::Sprite::Op(iscript[currOffset+1]) == Sc::Sprite::Op::goto_ && (u16 &)iscript[currOffset+2] == currOffset-1 ) // Wait-looped
                        {
                            if ( !context.isUnit )
                                currImage->end(); // "End" the image allowing sprites to auto-restart
                        }
                        advancePastParams();
                        return;
                    case Sc::Sprite::Op::waitrand:
                        {
                            if ( unbreak )
                            {
                                advancePastParams();
                                return;
                            }
                            auto first = iscript[currOffset+1];
                            auto second = iscript[currOffset+2];
                            auto min = std::min(first, second);
                            auto max = std::max(first, second);
                            currImage->waitUntil = context.currentTick + std::uint64_t(min + (std::rand() % (max-min)));
                            //waitUntil = currentTick;
                            advancePastParams();
                            return;
                        }
                    case Sc::Sprite::Op::goto_: // TODO: There are more such jump codes
                        {
                            u16 dest = (u16 &)iscript[currOffset];
                            if ( dest > iscript.size() )
                            {
                                //logger.error() << dest << iscript.size() << '\n';
                                return currImage->error("goto IScript Overflow");
                            }
                            else
                            {
                                currOffset = dest;
                                //logger.info() << "Goto " << currOffset << '\n';
                                currImage->animation = (Sc::Sprite::IScriptAnimation*)&iscript[dest];
                                continue;
                            }
                        }
                        break;
                    case Sc::Sprite::Op::imgol:
                        createOverlay((u16 &)iscript[currOffset], s8(iscript[currOffset+2])+currImage->xOffset, s8(iscript[currOffset+3])+currImage->yOffset, true);
                        break;
                    case Sc::Sprite::Op::imgul:
                        createOverlay((u16 &)iscript[currOffset], s8(iscript[currOffset+2])+currImage->xOffset, s8(iscript[currOffset+3])+currImage->yOffset, false);
                        break;
                    case Sc::Sprite::Op::imgolorig: // TODO
                        break;
                    case Sc::Sprite::Op::switchul: // TODO
                        break;
                    case Sc::Sprite::Op::unknown_0c: // TODO
                        break;
                    case Sc::Sprite::Op::imgoluselo: // TODO
                        {
                            u16 newImage = (u16 &)iscript[currOffset];
                            u8 overlayType = iscript[currOffset+2];
                            u8 direction = iscript[currOffset+3];
                            Sc::Sprite::LoOffset offset {};
                            const auto & loFiles = scData.sprites.loFiles;
                            const auto & imageDat = scData.sprites.getImage(currImage->imageId);
                            switch ( overlayType ) {
                                case Sc::Sprite::OverlayType::Attack: offset = loFiles[imageDat.attackOverlay].xyOffset(currImage->frame, direction); break;
                                case Sc::Sprite::OverlayType::Damage: offset = loFiles[imageDat.damageOverlay].xyOffset(currImage->frame, direction); break;
                                case Sc::Sprite::OverlayType::Special: offset = loFiles[imageDat.specialOverlay].xyOffset(currImage->frame, direction); break;
                                case Sc::Sprite::OverlayType::Landing: offset = loFiles[imageDat.landingDustOverlay].xyOffset(currImage->frame, direction); break;
                                case Sc::Sprite::OverlayType::Liftoff: offset = loFiles[imageDat.liftOffOverlay].xyOffset(currImage->frame, direction); break;
                            }
                            createOverlay(newImage, offset.xOffset+currImage->xOffset, offset.yOffset+currImage->yOffset, true);
                        }
                        break;
                    case Sc::Sprite::Op::imguluselo: // TODO
                        break;
                    case Sc::Sprite::Op::sprol:
                        createSpriteOverlay((u16 &)iscript[currOffset], iscript[currOffset+2], iscript[currOffset+3], true);
                        break;
                    case Sc::Sprite::Op::highsprol: // TODO
                        break;
                    case Sc::Sprite::Op::lowsprul: // TODO
                        break;
                    case Sc::Sprite::Op::uflunstable: // TODO
                        break;
                    case Sc::Sprite::Op::spruluselo: // TODO
                        break;
                    case Sc::Sprite::Op::sprul: // TODO
                        break;
                    case Sc::Sprite::Op::sproluselo: // TODO
                        break;
                    case Sc::Sprite::Op::end:
                        endImage();
                        return;
                    case Sc::Sprite::Op::setflipstate:
                        currImage->flipped = (iscript[currOffset] != 0);
                        break;
                    case Sc::Sprite::Op::playsnd: // TODO
                        break;
                    case Sc::Sprite::Op::playsndrand: // TODO
                        break;
                    case Sc::Sprite::Op::playsndbtwn: // TODO
                        break;
                    case Sc::Sprite::Op::domissiledmg: // TODO
                        break;
                    case Sc::Sprite::Op::attackmelee: // TODO
                        break;
                    case Sc::Sprite::Op::followmaingraphic:
                        {
                            MapImage* primaryImage = context.actor.primaryImage(context.animations);
                            if ( primaryImage != nullptr && (currImage->frame != primaryImage->frame || currImage->flipped != primaryImage->flipped) )
                            {
                                currImage->flipped = primaryImage->flipped;
                                currImage->baseFrame = primaryImage->baseFrame;
                                currImage->direction = primaryImage->direction;
                                currImage->frame = currImage->baseFrame + currImage->direction;
                            }
                        }
                        break;
                    case Sc::Sprite::Op::randcondjmp: // TODO
                        {
                            auto chance = iscript[currOffset];
                            //logger.info() << "Randcondjmp " << (u16 &)iscript[currOffset+1];
                            if ( std::rand() % 255 <= chance )
                            {
                                auto label = (u16 &)iscript[currOffset+1];
                                currOffset = label;
                                //logger << " taken\n";
                                currImage->animation = (Sc::Sprite::IScriptAnimation*)&iscript[label];
                                continue;
                            }
                        }
                        break;
                    case Sc::Sprite::Op::turnccwise:
                        if ( context.isUnit )
                            setActorDirection(context.actor.direction-8*iscript[currOffset]);
                        break;
                    case Sc::Sprite::Op::turncwise:
                        if ( context.isUnit )
                            setActorDirection(context.actor.direction+8*iscript[currOffset]);
                        break;
                    case Sc::Sprite::Op::turn1cwise:
                        if ( context.isUnit )
                            setActorDirection(context.actor.direction+8);
                        break;
                    case Sc::Sprite::Op::turnrand: // TODO
                        break;
                    case Sc::Sprite::Op::setspawnframe: // TODO
                        break;
                    case Sc::Sprite::Op::sigorder: // TODO
                        break;
                    case Sc::Sprite::Op::attackwith: // TODO
                        break;
                    case Sc::Sprite::Op::attack: // TODO
                        break;
                    case Sc::Sprite::Op::castspell: // TODO
                        break;
                    case Sc::Sprite::Op::useweapon: // TODO
                        break;
                    case Sc::Sprite::Op::move: // TODO
                        break;
                    case Sc::Sprite::Op::gotorepeatattk: // TODO
                        break;
                    case Sc::Sprite::Op::engframe: // TODO
                        break;
                    case Sc::Sprite::Op::engset: // TODO
                        break;
                    case Sc::Sprite::Op::unknown_2d: // TODO
                        break;
                    case Sc::Sprite::Op::nobrkcodestart:
                        context.actor.noBreakSection = true;
                        break;
                    case Sc::Sprite::Op::nobrkcodeend:
                        context.actor.noBreakSection = false;
                        unbreak = false;
                        break;
                    case Sc::Sprite::Op::ignorerest: // TODO
                        break;
                    case Sc::Sprite::Op::attkshiftproj: // TODO
                        break;
                    case Sc::Sprite::Op::tmprmgraphicstart: // TODO
                        break;
                    case Sc::Sprite::Op::tmprmgraphicend: // TODO
                        break;
                    case Sc::Sprite::Op::setfldirect:
                        if ( context.isUnit )
                            setActorDirection(iscript[currOffset]*8);
                        break;
                    case Sc::Sprite::Op::call:
                        {
                            auto label = (u16 &)iscript[currOffset];
                            advancePastParams();
                            currImage->returnOffset = currOffset;
                            currOffset = label;
                            //logger.info() << "Call " << currOffset << '\n';
                            currImage->animation = (Sc::Sprite::IScriptAnimation*)&iscript[label];
                        }
                        break;
                    case Sc::Sprite::Op::return_:
                        currOffset = currImage->returnOffset;
                        currImage->animation = (Sc::Sprite::IScriptAnimation*)&iscript[currOffset];
                        break;
                    case Sc::Sprite::Op::setflspeed: // TODO
                        break;
                    case Sc::Sprite::Op::creategasoverlays: // TODO
                        break;
                    case Sc::Sprite::Op::pwrupcondjmp: // TODO
                        break;
                    case Sc::Sprite::Op::trgtrangecondjmp: // TODO
                        break;
                    case Sc::Sprite::Op::trgtarccondjmp: // TODO
                        break;
                    case Sc::Sprite::Op::curdirectcondjmp: // TODO
                        break;
                    case Sc::Sprite::Op::imgulnextid: // TODO
                        break;
                    case Sc::Sprite::Op::unknown_3e: // TODO
                        break;
                    case Sc::Sprite::Op::liftoffcondjmp: // TODO
                        break;
                    case Sc::Sprite::Op::warpoverlay: // TODO
                        break;
                    case Sc::Sprite::Op::orderdone: // TODO
                        break;
                    case Sc::Sprite::Op::grdsprol: // TODO
                        break;
                    case Sc::Sprite::Op::unknown_43: // TODO
                        break;
                    case Sc::Sprite::Op::dogrddamage: // TODO
                        break;
                    default:
                        break;
                }
                advancePastParams();
            }
        }
    }
}
