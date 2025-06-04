#ifndef ACTIONDESCRIPTOR_H
#define ACTIONDESCRIPTOR_H

enum class ActionDescriptor
{
    None = 0,
    UpdateUnitSel,
    ClearUnitSel,
    CreateUnit,
    PasteUnits,
    BrushIsom
};

struct DescriptorIndex {
    ActionDescriptor descriptorIndex = ActionDescriptor::None;
    constexpr DescriptorIndex() noexcept = default;
    constexpr DescriptorIndex(ActionDescriptor actionDescriptor) noexcept : descriptorIndex(actionDescriptor) {}
    friend constexpr bool operator==(const DescriptorIndex & lhs, const DescriptorIndex & rhs) noexcept { return lhs.descriptorIndex == rhs.descriptorIndex; }
};

#endif