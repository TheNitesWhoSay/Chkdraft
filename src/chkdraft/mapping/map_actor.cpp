#include "map_actor.h"
#include "../chkdraft.h"

MapActor & MapActor::operator=(const MapActor & other)
{
    this->mapImages = other.mapImages;
    this->returnOffset = other.returnOffset;
    this->iScriptId = other.iScriptId;
    this->animation = other.animation;
    this->waitUntil = other.waitUntil;
    std::copy(std::begin(other.usedImages), std::end(other.usedImages), this->usedImages);
    return *this;
}

void MapActor::initialize(std::uint64_t currentTick, size_t iScriptId, bool isUnit)
{
    this->iScriptId = iScriptId;
    animation = chkd.scData.sprites.getAnimationHeader(iScriptId);
    if ( animation == nullptr )
        end();
    else
    {
        waitUntil = currentTick;
        animate(currentTick, isUnit); // Advance until the first wait such that the image starts on the correct frame and all
    }
}

bool MapActor::end()
{
    waitUntil = std::numeric_limits<uint64_t>::max();
    return true;
}

void MapActor::error(std::string_view message)
{
    logger.error(message);
    end();
}

void MapActor::restartIfEnded(std::uint64_t currentTick, bool isUnit)
{
    if ( waitUntil == std::numeric_limits<uint64_t>::max() )
        initialize(currentTick, this->iScriptId, isUnit);
}

void MapActor::advanceBy(size_t numBytes)
{
    ((u8* &)animation) += numBytes;
}

void MapActor::animate(std::uint64_t currentTick, bool isUnit)
{
    auto & iscript = chkd.scData.sprites.iscript;
    size_t currOffset = std::distance((const u8*)&iscript[0], (const u8*)animation);
    if ( currentTick >= waitUntil )
    {
        // Step into the next or couple next iscript ops
        for ( ; ; )
        {
            if ( currOffset >= iscript.size() )
                return error("IScript Overflow");

            Sc::Sprite::Op code = Sc::Sprite::Op(animation->code);
            ++currOffset; // 1-byte code
            std::string opName = code < Sc::Sprite::OpName.size() ? std::string(Sc::Sprite::OpName[code]) : std::to_string(int(code));
            //logger.info() << "  " << opName << ", \n";

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
                        mapImages[usedImages[0]]->playFrame(iscript[currOffset]);
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
                            mapImages[usedImages[0]]->frame = 0;
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
                                return error("IScript Overflow");
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
                    case Sc::Sprite::Op::imgol: // TODO
                        break;
                    case Sc::Sprite::Op::imgul: // TODO
                        break;
                    case Sc::Sprite::Op::imgolorig: // TODO
                        break;
                    case Sc::Sprite::Op::switchul: // TODO
                        break;
                    case Sc::Sprite::Op::unknown_0c: // TODO
                        break;
                    case Sc::Sprite::Op::imgoluselo: // TODO
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
                        mapImages[usedImages[0]]->flipped = (iscript[currOffset] != 0);
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

void MapActor::setDirection(u8 direction)
{
    this->direction = direction;
    mapImages[usedImages[0]]->setDirection(direction);
}
