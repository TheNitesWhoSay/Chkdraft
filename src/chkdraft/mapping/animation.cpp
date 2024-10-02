#include "animation.h"
#include "../chkdraft.h"

void IscriptAnim::initialize(std::uint64_t currentTick, size_t iScriptId)
{
    this->iScriptId = iScriptId;
    animation = chkd.scData.sprites.getAnimationHeader(iScriptId);
    if ( animation == nullptr )
        end();
    else
    {
        waitUntil = currentTick;
        animate(currentTick); // Advance until the first wait such that the image starts on the correct frame and all
    }
}

bool IscriptAnim::end()
{
    waitUntil = std::numeric_limits<uint64_t>::max();
    return true;
}

void IscriptAnim::error(std::string_view message)
{
    logger.error(message);
    end();
}

void IscriptAnim::restartIfEnded(std::uint64_t currentTick)
{
    if ( waitUntil == std::numeric_limits<uint64_t>::max() )
        initialize(currentTick, this->iScriptId);
}

void IscriptAnim::advanceBy(size_t numBytes)
{
    ((u8* &)animation) += numBytes;
}

void IscriptAnim::animate(std::uint64_t currentTick)
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
                        this->frame = iscript[currOffset];
                        break;
                    case Sc::Sprite::Op::setflipstate:
                        //bool flipped = (iscript[currOffset] == 1);
                        // TODO: set some kinda flipped state
                        break;
                    case Sc::Sprite::Op::sprol:
                        this->xOffset = s32(s8(iscript[currOffset+2]));
                        this->yOffset = s32(s8(iscript[currOffset+3]));
                        this->frame++;
                        break;
                    case Sc::Sprite::Op::call:
                        {
                            auto label = (u16 &)iscript[currOffset];
                            advancePastParams();
                            this->returnOffset = currOffset;
                            currOffset = label;
                            this->animation = (Sc::Sprite::IScriptAnimation*)&iscript[label];
                        }
                        break;
                    case Sc::Sprite::Op::return_:
                        currOffset = this->returnOffset;
                        this->animation = (Sc::Sprite::IScriptAnimation*)&iscript[currOffset];
                        break;
                    case Sc::Sprite::Op::end:
                        return;
                    case Sc::Sprite::Op::wait:
                        waitUntil = currentTick + uint64_t(iscript[currOffset]);
                        if ( Sc::Sprite::Op(iscript[currOffset+1]) == Sc::Sprite::Op::goto_ && (u16 &)iscript[currOffset+2] == currOffset-1 )
                        {
                            // wait-looped, you don't necessarily want to end here but it permits restarting
                            // TODO: Just because you're wait-looped doesn't mean restarting is appropriate...
                            frame = 0;
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
                            advancePastParams();
                            return;
                        }
                    case Sc::Sprite::Op::goto_: // TODO: There are more such jump codes
                        {
                            u16 dest = (u16 &)iscript[currOffset];
                            if ( dest > iscript.size() )
                            {
                                logger.error() << dest << iscript.size() << '\n';
                                return error("IScript Overflow");
                            }
                            else
                            {
                                currOffset = dest;
                                this->animation = (Sc::Sprite::IScriptAnimation*)&iscript[dest];
                                continue;
                            }
                        }
                        break;
                    default:
                        break;
                }
                advancePastParams();
            }
        }
    }
}