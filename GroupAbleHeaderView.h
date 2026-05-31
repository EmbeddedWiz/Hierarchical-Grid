#ifndef GROUPABLEHEADERVIEW_H
#define GROUPABLEHEADERVIEW_H

#include <QHeaderView>
#include <QSet>
#include <QMap>

class GroupedTableView;

/**
 * @class GroupableHeaderView
 * @brief A custom QHeaderView subclass that supports interactive column/row grouping.
 *
 * This header view draws Excel-like expand/collapse (+/-) buttons and connector
 * lines. Clicking the button emits signals to collapse or expand grouped rows/columns.
 */
class GroupableHeaderView : public QHeaderView
{
    Q_OBJECT
public:
    /**
     * @brief Constructs a GroupableHeaderView.
     * @param orientation The orientation of the header (Qt::Horizontal or Qt::Vertical).
     * @param parent The parent widget.
     */
    explicit GroupableHeaderView(Qt::Orientation orientation, QWidget *parent = nullptr);

    /**
     * @brief Adds a grouping indicator (button) to a specific logical section.
     * @param logicalIndex The 0-based index of the column or row containing the toggle button.
     */
    void addGroup(int logicalIndex);

    /**
     * @brief Removes the grouping indicator and state from a specific logical section.
     * @param logicalIndex The 0-based index of the column or row to remove grouping from.
     */
    void removeGroup(int logicalIndex);

    /**
     * @brief Checks if a logical section is configured as a group trigger.
     * @param logicalIndex The logical section index.
     * @return True if the section has a grouping button, false otherwise.
     */
    [[nodiscard]] bool hasGroup(int logicalIndex) const;

    /**
     * @brief Sets the collapse state of a grouping section.
     * @param logicalIndex The logical section index.
     * @param collapsed True to collapse the group, false to expand.
     */
    void setCollapsed(int logicalIndex, bool collapsed);

    /**
     * @brief Checks if a grouping section is currently collapsed.
     * @param logicalIndex The logical section index.
     * @return True if the group is collapsed, false if expanded or not a group.
     */
    [[nodiscard]] bool isCollapsed(int logicalIndex) const;

    /**
     * @brief Clears all groups and their collapse states.
     */
    void clearGroups();

signals:
    /**
     * @brief Emitted when a grouping button is clicked, triggering a state toggle.
     * @param logicalIndex The logical index of the column or row that was toggled.
     * @param collapsed The new collapse state (true for collapsed, false for expanded).
     */
    void groupToggled(int logicalIndex, bool collapsed);

protected:
    /**
     * @brief Paints the header section, including custom expand/collapse buttons.
     */
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override;

    /**
     * @brief Handles mouse press events to capture clicks on the expand/collapse buttons.
     */
    void mousePressEvent(QMouseEvent *event) override;

    /**
     * @brief Computes the size hint of a section based on its content, adjusting for the buttons.
     */
    QSize sectionSizeFromContents(int logicalIndex) const override;

    /**
     * @brief Custom paint event that draws the connector brackets behind/around sections.
     */
    void paintEvent(QPaintEvent *event) override;

private:
    /**
     * @brief Computes the bounding rectangle for the +/- button in a given section.
     * @param sectionRect The bounding rectangle of the header section.
     * @return The rectangle where the button should be drawn.
     */
    QRect buttonRect(const QRect &sectionRect) const;

    /**
     * @brief Returns the size of the +/- button square in pixels.
     * @return The button size (13px).
     */
    int buttonSize() const { return 13; }

    /**
     * @brief Draws a single +/- expand/collapse button.
     * @param painter The active QPainter.
     * @param btnRect The target rectangle for the button.
     * @param collapsed The collapse state (determines whether to draw + or -).
     */
    void drawGroupButton(QPainter *painter, const QRect &btnRect, bool collapsed) const;

    /**
     * @brief Draws the vertical connector line (bracket) for a row group.
     * @param painter The active QPainter.
     * @param trigger The row index triggering the group.
     */
    void drawRowGroupLine(QPainter *painter, int trigger);

    /**
     * @brief Draws the horizontal connector line (bracket) for a column group.
     * @param painter The active QPainter.
     * @param trigger The column index triggering the group.
     */
    void drawColumnGroupLine(QPainter *painter, int trigger);

    QList<int> m_groups;           ///< List of logical indices that have grouping buttons.
    QMap<int, bool> m_collapsed;   ///< Maps logical index of group trigger to collapse state (true if collapsed).
};

#endif // GROUPABLEHEADERVIEW_H
