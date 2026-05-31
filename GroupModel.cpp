/**
 * @file GroupModel.cpp
 * @brief Implementation of the logical model for managing row and column grouping.
 */

#include "GroupModel.h"

namespace {
    // Comparison operators for strongly-typed indices
    constexpr bool operator==(const RowIdx& lhs, const RowIdx& rhs) noexcept {
        return lhs.value == rhs.value;
    }
    constexpr bool operator==(const ColIdx& lhs, const ColIdx& rhs) noexcept {
        return lhs.value == rhs.value;
    }
}

bool GroupModel::addRowGroup(RowIdx trigger, RowIdx to) noexcept {
    // Trigger index must be non-negative and sit before or at the end of the group
    if (trigger.value < 0 || trigger > to) 
        return false;

    // Check for overlap with any existing row groups
    for (const auto& g : m_rowGroups) {
        if (!(to < g.triggerRow || trigger > g.to)) {
            return false; // Found overlapping ranges
        }
    }
    m_rowGroups.push_back({trigger, to});
    return true;
}

bool GroupModel::addColumnGroup(ColIdx trigger, ColIdx to) noexcept {
    // Trigger index must be non-negative and sit before or at the end of the group
    if (trigger.value < 0 || trigger > to) 
        return false;

    // Check for overlap with any existing column groups
    for (const auto& g : m_colGroups) {
        if (!(to < g.triggerCol || trigger > g.to)) {
            return false; // Found overlapping ranges
        }
    }
    m_colGroups.push_back({trigger, to});
    return true;
}

void GroupModel::removeRowGroup(RowIdx trigger) noexcept {
    // Remove the group from the registered list of row groups
    std::erase_if(m_rowGroups, [&](const RowGroup& g){ return g.triggerRow == trigger; });
    // Remove its collapse/expand state
    m_rowCollapsed.erase(trigger.value);
}

void GroupModel::removeColumnGroup(ColIdx trigger) noexcept {
    // Remove the group from the registered list of column groups
    std::erase_if(m_colGroups, [&](const ColumnGroup& g){ return g.triggerCol == trigger; });
    // Remove its collapse/expand state
    m_colCollapsed.erase(trigger.value);
}

void GroupModel::setRowCollapsed(RowIdx trigger, bool collapsed) noexcept {
    // Update or insert collapse state for the row group
    if (auto it = m_rowCollapsed.find(trigger.value); it != m_rowCollapsed.end()) {
        it->second = collapsed;
    } else {
        m_rowCollapsed.emplace(trigger.value, collapsed);
    }
}

void GroupModel::setColumnCollapsed(ColIdx trigger, bool collapsed) noexcept {
    // Update or insert collapse state for the column group
    if (auto it = m_colCollapsed.find(trigger.value); it != m_colCollapsed.end()) {
        it->second = collapsed;
    } else {
        m_colCollapsed.emplace(trigger.value, collapsed);
    }
}

bool GroupModel::isRowCollapsed(RowIdx trigger) const noexcept {
    // Retrieve collapsed state; defaults to expanded (false) if not found
    if (auto it = m_rowCollapsed.find(trigger.value); it != m_rowCollapsed.end()) {
        return it->second;
    }
    return false;
}

bool GroupModel::isColumnCollapsed(ColIdx trigger) const noexcept {
    // Retrieve collapsed state; defaults to expanded (false) if not found
    if (auto it = m_colCollapsed.find(trigger.value); it != m_colCollapsed.end()) {
        return it->second;
    }
    return false;
}

void GroupModel::clearRowGroups() noexcept {
    m_rowGroups.clear();
    m_rowCollapsed.clear();
}

void GroupModel::clearColumnGroups() noexcept {
    m_colGroups.clear();
    m_colCollapsed.clear();
}
