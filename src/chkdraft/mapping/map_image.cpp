#include <mapping_core/map_image.h>
#include "../chkdraft.h" // TODO: recheck whether there's a way this file could go in mapping_core too

u32 iscriptIdFromImage(u32 imageId)
{
    return chkd.scData.sprites.getImage(imageId).iScriptId;
}

void MapImage::error(std::string_view message)
{
    logger.error(message);
    end();
}

void MapImage::initialize(std::uint64_t currentTick, size_t iScriptId, MapAnimations & animations, MapActor & actor, bool isUnit)
{
    this->iScriptId = iScriptId;
    animation = chkd.scData.sprites.getAnimationHeader(iScriptId);
    if ( animation == nullptr )
        end();
    else
    {
        waitUntil = currentTick;
        animate(currentTick, animations, actor, isUnit); // Advance until the first wait such that the image starts on the correct frame and all
    }
}

void MapImage::restartIfEnded(std::uint64_t currentTick, MapAnimations & animations, MapActor & actor, bool isUnit)
{
    if ( waitUntil == std::numeric_limits<uint64_t>::max() )
        initialize(currentTick, this->iScriptId, animations, actor, isUnit);
}

bool MapImage::end()
{
    waitUntil = std::numeric_limits<uint64_t>::max();
    return true;
}


void MapImage::playFrame(u8 frame)
{
    if ( this->baseFrame != frame )
    {
        this->baseFrame = frame;
        this->frame = this->baseFrame + this->direction;
    }
}

void MapImage::setDirection(u8 direction)
{
    this->direction = (direction+4)/8;
    this->flipped = this->direction > 16;
    if ( flipped )
        this->direction = 32-this->direction;

    frame = baseFrame + this->direction;
}

void MapImage::createOverlay(u16 imageId, s8 x, s8 y, MapAnimations & animations, MapActor & actor, bool isUnit, bool above)
{
    MapImage* primaryImage = actor.primaryImage(animations);
    if ( primaryImage == nullptr )
    {
        logger.warn("CreateOverlay called but there was no primary image for the actor.");
        return;
    }
    auto & overlayImageIndex = actor.getNewImageSlot(above, *this, animations);
    if ( overlayImageIndex != 0 )
    {
        logger.warn("CreateOverlay called but no image slots were available for the actor.");
        return;
    }
    overlayImageIndex = animations.createImage();
    //logger.info() << "PrimaryImage: " << usedImages[0] << ", " << primaryImage.xc << ", " << primaryImage.yc << '\n';
    auto & overlayImage = animations.images[overlayImageIndex].value();
    overlayImage.imageId = imageId;
    overlayImage.xc = primaryImage->xc + s32(x);
    overlayImage.yc = primaryImage->yc + s32(y);
    overlayImage.drawFunction = (MapImage::DrawFunction)chkd.scData.sprites.getImage(imageId).drawFunction;
    logger.info() << this << " creating overlay: " << &overlayImage << '\n';
    overlayImage.initialize(chkd.gameClock.currentTick(), iscriptIdFromImage(imageId), animations, actor, isUnit);
    //logger.info() << "CreateOverlay: " << overlayImageIndex << ", " << imageId << ", " << "(" << overlayImage.xc << ", " << overlayImage.yc << ")\n";
}

void MapImage::advanceBy(size_t numBytes)
{
    ((u8* &)animation) += numBytes;
}

void MapImage::animate(std::uint64_t currentTick, MapAnimations & animations, MapActor & actor, bool isUnit)
{
    auto & images = animations.images;
    auto & iscript = chkd.scData.sprites.iscript;
    size_t currOffset = std::distance((const u8*)&iscript[0], (const u8*)animation);
    if ( currentTick >= waitUntil )
    {
        // Step into the next or couple next iscript ops
        for ( ; ; )
        {
            if ( currOffset >= iscript.size() )
            {
                logger.info() << "  " << this << '\n';
                return error("Unknown IScript Overflow");
            }

            Sc::Sprite::Op code = Sc::Sprite::Op(animation->code);
            ++currOffset; // 1-byte code
            std::string opName = code < Sc::Sprite::OpName.size() ? std::string(Sc::Sprite::OpName[code]) : std::to_string(int(code));
            //logger.info() << "  " << this << ", " << opName << ", \n";

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
                        animation = (Sc::Sprite::IScriptAnimation*)&iscript[currOffset];
                };
                switch ( code )
                {
                    case Sc::Sprite::Op::playfram:
                        playFrame(iscript[currOffset]);
                        break;
                    case Sc::Sprite::Op::playframtile: // TODO
                        break;
                    case Sc::Sprite::Op::sethorpos: // TODO
                        break;
                    case Sc::Sprite::Op::setvertpos: // TODO
                        break;
                    case Sc::Sprite::Op::setpos: // TODO
                        break;
                    case Sc::Sprite::Op::wait:
                        waitUntil = currentTick + uint64_t(iscript[currOffset]);
                        
                        //waitUntil = currentTick;
                        if ( Sc::Sprite::Op(iscript[currOffset+1]) == Sc::Sprite::Op::goto_ && (u16 &)iscript[currOffset+2] == currOffset-1 )
                        {
                            // wait-looped, you don't necessarily want to end here but it permits restarting
                            // TODO: Just because you're wait-looped doesn't mean restarting is appropriate...
                            //logger.info() << "WaitLoopRestart\n";
                            //frame = 0;
                            end();
                            return;
                        }
                        advancePastParams();
                        return;
                    case Sc::Sprite::Op::waitrand:
                        {
                            auto first = iscript[currOffset+1];
                            auto second = iscript[currOffset+2];
                            auto min = std::min(first, second);
                            auto max = std::max(first, second);
                            waitUntil = currentTick + uint64_t(min + (std::rand() % (max-min)));
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
                                return error("goto IScript Overflow");
                            }
                            else
                            {
                                currOffset = dest;
                                //logger.info() << "Goto " << currOffset << '\n';
                                this->animation = (Sc::Sprite::IScriptAnimation*)&iscript[dest];
                                continue;
                            }
                        }
                        break;
                    case Sc::Sprite::Op::imgol:
                        createOverlay((u16 &)iscript[currOffset], s8(iscript[currOffset+2])+xOffset, s8(iscript[currOffset+3])+yOffset, animations, actor, isUnit, true);
                        break;
                    case Sc::Sprite::Op::imgul:
                        createOverlay((u16 &)iscript[currOffset], s8(iscript[currOffset+2])+xOffset, s8(iscript[currOffset+3])+yOffset, animations, actor, isUnit, false);
                        break;
                    case Sc::Sprite::Op::imgolorig: // TODO
                        break;
                    case Sc::Sprite::Op::switchul: // TODO
                        break;
                    case Sc::Sprite::Op::unknown_0c: // TODO
                        break;
                    case Sc::Sprite::Op::imgoluselo: // TODO
                        createOverlay((u16 &)iscript[currOffset], 0+xOffset, 0+yOffset, animations, actor, isUnit, true);
                        break;
                    case Sc::Sprite::Op::imguluselo: // TODO
                        break;
                    case Sc::Sprite::Op::sprol: // TODO
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
                        // TODO: this is called for a non-main image to destroy said image
                        return;
                    case Sc::Sprite::Op::setflipstate:
                        this->flipped = (iscript[currOffset] != 0);
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
                    case Sc::Sprite::Op::followmaingraphic: // TODO
                        {
                            MapImage* primaryImage = actor.primaryImage(animations);
                            if ( primaryImage != nullptr && (this->baseFrame != primaryImage->baseFrame || this->flipped != primaryImage->flipped) )
                            {
                                this->baseFrame = primaryImage->baseFrame;
                                this->flipped = primaryImage->flipped;
                                this->direction = primaryImage->direction;
                                this->setDirection(this->direction);
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
                                this->animation = (Sc::Sprite::IScriptAnimation*)&iscript[label];
                                continue;
                            }
                        }
                        break;
                    case Sc::Sprite::Op::turnccwise: // TODO
                        if ( isUnit )
                        {
                            //logger.info("turn counter-clockwise");
                            setDirection(this->direction-8*iscript[currOffset]);
                        }
                        break;
                    case Sc::Sprite::Op::turncwise: // TODO
                        if ( isUnit )
                        {
                            //logger.info("turn clockwise");
                            setDirection(this->direction+8*iscript[currOffset]);
                        }
                        break;
                    case Sc::Sprite::Op::turn1cwise: // TODO
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
                    case Sc::Sprite::Op::nobrkcodestart: // TODO
                        break;
                    case Sc::Sprite::Op::nobrkcodeend: // TODO
                        break;
                    case Sc::Sprite::Op::ignorerest: // TODO
                        break;
                    case Sc::Sprite::Op::attkshiftproj: // TODO
                        break;
                    case Sc::Sprite::Op::tmprmgraphicstart: // TODO
                        break;
                    case Sc::Sprite::Op::tmprmgraphicend: // TODO
                        break;
                    case Sc::Sprite::Op::setfldirect: // TODO
                        break;
                    case Sc::Sprite::Op::call:
                        {
                            auto label = (u16 &)iscript[currOffset];
                            advancePastParams();
                            this->returnOffset = currOffset;
                            currOffset = label;
                            //logger.info() << "Call " << currOffset << '\n';
                            this->animation = (Sc::Sprite::IScriptAnimation*)&iscript[label];
                        }
                        break;
                    case Sc::Sprite::Op::return_:
                        currOffset = this->returnOffset;
                        this->animation = (Sc::Sprite::IScriptAnimation*)&iscript[currOffset];
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
