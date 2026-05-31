#ifndef GROUPEDTABLEVIEW_H
#define GROUPEDTABLEVIEW_H

#include "GroupAbleHeaderView.h"
#include "GroupModel.h"
#include <QStandardItemModel>
#include <QTableView>

class GroupableHeaderView;

/**
 * @class GroupedTableView
 * @brief A QTableView extension that integrates with GroupModel and GroupableHeaderView to support grouping.
 *
 * This table view manages row and column grouping. It coordinates the display of collapse/expand indicators
 * in the headers and hides/shows rows and columns when groups are toggled.
 */
class GroupedTableView : public QTableView
{
    Q_OBJECT

public:
    /**
     * @brief Returns a read-only reference to the list of row groups.
     * @return A vector of RowGroup objects.
     */
    [[nodiscard]] const std::vector<RowGroup>& rowGroups() const noexcept { return m_groupModel.rowGroups(); }

    /**
     * @brief Returns a read-only reference to the list of column groups.
     * @return A vector of ColumnGroup objects.
     */
    [[nodiscard]] const std::vector<ColumnGroup>& columnGroups() const noexcept { return m_groupModel.columnGroups(); }

    /**
     * @brief Constructs a GroupedTableView.
     * @param parent The parent widget.
     */
    explicit GroupedTableView(QWidget *parent = nullptr);

    /**
     * @brief Computes the ideal size to display the entire table content without scrollbars.
     * @return The size hint of the table.
     */
    [[nodiscard]] QSize sizeHint() const override;

    /**
     * @brief Factory method to create, initialize, and return a GroupedTableView.
     * @param rows The number of rows.
     * @param cols The number of columns.
     * @param parent The parent widget.
     * @return A pointer to the newly created and initialized GroupedTableView.
     */
    static GroupedTableView* create(int rows, int cols, QWidget *parent);

    /**
     * @brief Initializes the table view with a default QStandardItemModel.
     * @param rows The number of rows.
     * @param cols The number of columns.
     * @param columnNames Optional custom header titles for the columns.
     */
    virtual void init(int rows, int cols, const QStringList& columnNames = {});

    /**
     * @brief Removes all column groups, restoring visibility to all columns.
     */
    void removeAllColumnGroups();

    /**
     * @brief Removes all row groups, restoring visibility to all rows.
     */
    void removeAllRowGroups();

    /**
     * @brief Groups a range of rows.
     * @param triggerRow The 0-based index of the row containing the expand/collapse button.
     * @param to The end row index of the group (inclusive).
     */
    void groupRow(int triggerRow, int to);

    /**
     * @brief Groups a range of columns.
     * @param triggerCol The 0-based index of the column containing the expand/collapse button.
     * @param to The end column index of the group (inclusive).
     */
    void groupColumn(int triggerCol, int to);

    /**
     * @brief Fills the table cells with placeholder test data (e.g. "Cell A1").
     */
    void fillTestData();

    /**
     * @brief Collapses all row groups.
     */
    void collapseAllRowGroups();

    /**
     * @brief Expands all row groups.
     */
    void expandAllRowGroups();

    /**
     * @brief Collapses all column groups.
     */
    void collapseAllColumnGroups();

    /**
     * @brief Expands all column groups.
     */
    void expandAllColumnGroups();

    /**
     * @brief Removes a single row group.
     * @param triggerRow The trigger row index of the group to remove.
     */
    void removeGroupRow(int triggerRow);

    /**
     * @brief Removes a single column group.
     * @param triggerCol The trigger column index of the group to remove.
     */
    void removeGroupColumn(int triggerCol);

private:
    /**
     * @brief Gets the horizontal group header view.
     */
    GroupableHeaderView* horizontalGroupHeader() const;

    /**
     * @brief Gets the vertical group header view.
     */
    GroupableHeaderView* verticalGroupHeader() const;

    /**
     * @brief Helper function to convert a column index to an Excel-style letter (e.g. 0 -> "A", 27 -> "AB").
     * @param col The 0-based column index.
     * @return The Excel-style column name string.
     */
    static QString columnLetter(int col);

    GroupModel m_groupModel;           ///< Core grouping logic and validation model.
    GroupableHeaderView *m_hHeader;    ///< Custom horizontal header.
    GroupableHeaderView *m_vHeader;    ///< Custom vertical header.
};

#endif // GROUPEDTABLEVIEW_H
