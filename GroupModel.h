/**
 * @file GroupModel.h
 * @brief Pure C++20 model for managing hierarchical groups in a table.
 *
 * This class is independent of Qt and can be unit-tested in isolation.
 */
#pragma once
#include <vector>
#include <unordered_map>
#include <ranges>
#include <cassert>

/** 
 * @struct RowIdx
 * @brief Strongly-typed index wrapper for rows.
 */
struct RowIdx {
    int value;
    constexpr explicit RowIdx(int v) : value(v) {}
    constexpr operator int() const noexcept { return value; }
};

/** 
 * @struct ColIdx
 * @brief Strongly-typed index wrapper for columns.
 */
struct ColIdx {
    int value;
    constexpr explicit ColIdx(int v) : value(v) {}
    constexpr operator int() const noexcept { return value; }
};

/** 
 * @struct RowGroup
 * @brief Represents a contiguous range of rows belonging to a group.
 */
struct RowGroup {
    RowIdx triggerRow;   ///< Row with the toggle button.
    RowIdx to;           ///< Last row of the group (inclusive).
};

/** 
 * @struct ColumnGroup
 * @brief Represents a contiguous range of columns belonging to a group.
 */
struct ColumnGroup {
    ColIdx triggerCol;   ///< Column with the toggle button.
    ColIdx to;           ///< Last column of the group (inclusive).
};

/**
 * @class GroupModel
 * @brief Holds grouping data and provides validation/collapse tracking.
 *
 * The model does not perform any UI operations. It merely tracks which groups
 * exist and whether they are currently collapsed.
 */
class GroupModel {
public:
    /**
     * @brief Adds a row group if the range is valid and does not overlap existing groups.
     * @param trigger The row index displaying the expand/collapse button.
     * @param to The final row index in the group.
     * @return True if the group was successfully added, false otherwise.
     */
    [[nodiscard]] bool addRowGroup(RowIdx trigger, RowIdx to) noexcept;

    /**
     * @brief Adds a column group if the range is valid and does not overlap existing groups.
     * @param trigger The column index displaying the expand/collapse button.
     * @param to The final column index in the group.
     * @return True if the group was successfully added, false otherwise.
     */
    [[nodiscard]] bool addColumnGroup(ColIdx trigger, ColIdx to) noexcept;

    /**
     * @brief Removes a row group by its trigger index.
     * @param trigger The trigger row index of the group to remove.
     */
    void removeRowGroup(RowIdx trigger) noexcept;

    /**
     * @brief Removes a column group by its trigger index.
     * @param trigger The trigger column index of the group to remove.
     */
    void removeColumnGroup(ColIdx trigger) noexcept;

    /**
     * @brief Sets the collapsed state of a row group.
     * @param trigger The trigger row index of the group.
     * @param collapsed True to collapse, false to expand.
     */
    void setRowCollapsed(RowIdx trigger, bool collapsed) noexcept;

    /**
     * @brief Sets the collapsed state of a column group.
     * @param trigger The trigger column index of the group.
     * @param collapsed True to collapse, false to expand.
     */
    void setColumnCollapsed(ColIdx trigger, bool collapsed) noexcept;

    /**
     * @brief Checks if a row group is currently collapsed.
     * @param trigger The trigger row index of the group.
     * @return True if collapsed, false if expanded or not found.
     */
    [[nodiscard]] bool isRowCollapsed(RowIdx trigger) const noexcept;

    /**
     * @brief Checks if a column group is currently collapsed.
     * @param trigger The trigger column index of the group.
     * @return True if collapsed, false if expanded or not found.
     */
    [[nodiscard]] bool isColumnCollapsed(ColIdx trigger) const noexcept;

    /**
     * @brief Returns a read-only reference to the list of row groups.
     * @return Const reference to row groups vector.
     */
    [[nodiscard]] const std::vector<RowGroup>& rowGroups() const noexcept { return m_rowGroups; }

    /**
     * @brief Returns a read-only reference to the list of column groups.
     * @return Const reference to column groups vector.
     */
    [[nodiscard]] const std::vector<ColumnGroup>& columnGroups() const noexcept { return m_colGroups; }

    /**
     * @brief Returns a mutable reference to the list of row groups.
     * @return Reference to row groups vector.
     */
    [[nodiscard]] std::vector<RowGroup>& rowGroups() noexcept { return m_rowGroups; }

    /**
     * @brief Returns a mutable reference to the list of column groups.
     * @return Reference to column groups vector.
     */
    [[nodiscard]] std::vector<ColumnGroup>& columnGroups() noexcept { return m_colGroups; }

    /**
     * @brief Clears all registered row groups and their collapsed states.
     */
    void clearRowGroups() noexcept;

    /**
     * @brief Clears all registered column groups and their collapsed states.
     */
    void clearColumnGroups() noexcept;

private:
    std::unordered_map<int, bool> m_rowCollapsed;   ///< Maps row trigger index to collapsed state.
    std::unordered_map<int, bool> m_colCollapsed;   ///< Maps column trigger index to collapsed state.
    std::vector<RowGroup> m_rowGroups;              ///< List of active row groups.
    std::vector<ColumnGroup> m_colGroups;           ///< List of active column groups.
};
