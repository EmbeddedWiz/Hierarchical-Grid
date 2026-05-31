#include "GroupedTableView.h"
#include <QHeaderView>

GroupedTableView::GroupedTableView(QWidget *parent)
    : QTableView(parent)
{
    m_hHeader = new GroupableHeaderView(Qt::Horizontal, this);
    m_vHeader = new GroupableHeaderView(Qt::Vertical, this);

    setHorizontalHeader(m_hHeader);
    setVerticalHeader(m_vHeader);

    // Connect vertical header toggling to hide/show rows
    connect(m_vHeader, &GroupableHeaderView::groupToggled,
            this, [this](int logicalRow, bool collapsed) {
                for (const auto &g : std::as_const(m_groupModel.rowGroups())) {
                    if (g.triggerRow == logicalRow) {
                        for (int i = g.triggerRow + 1; i <= g.to; ++i)
                            setRowHidden(i, collapsed);
                        break;
                    }
                }
            });

    // Connect horizontal header toggling to hide/show columns
    connect(m_hHeader, &GroupableHeaderView::groupToggled,
            this, [this](int logicalCol, bool collapsed) {
                for (const auto &g : std::as_const(m_groupModel.columnGroups())) {
                    if (g.triggerCol == logicalCol) {
                        for (int i = g.triggerCol + 1; i <= g.to; ++i)
                            setColumnHidden(i, collapsed);
                        break;
                    }
                }
            });
}

GroupableHeaderView* GroupedTableView::horizontalGroupHeader() const 
{ 
    return m_hHeader; 
}

GroupableHeaderView* GroupedTableView::verticalGroupHeader() const 
{ 
    return m_vHeader; 
}

void GroupedTableView::removeGroupRow(int triggerRow)
{
    m_groupModel.removeRowGroup(RowIdx(triggerRow));
    m_vHeader->removeGroup(triggerRow);
    viewport()->update();
    verticalHeader()->viewport()->update();
}

void GroupedTableView::removeGroupColumn(int triggerCol)
{
    m_groupModel.removeColumnGroup(ColIdx(triggerCol));
    m_hHeader->removeGroup(triggerCol);
    viewport()->update();
    horizontalHeader()->viewport()->update();
}

void GroupedTableView::groupRow(int triggerRow, int to)
{
    if (m_groupModel.addRowGroup(RowIdx(triggerRow), RowIdx(to)))
        m_vHeader->addGroup(triggerRow);
}

void GroupedTableView::groupColumn(int triggerCol, int to)
{
    if (m_groupModel.addColumnGroup(ColIdx(triggerCol), ColIdx(to)))
        m_hHeader->addGroup(triggerCol);
}

QString GroupedTableView::columnLetter(int col)
{
    QString s;
    col++;
    while (col > 0) {
        col--;
        s.prepend(QChar('A' + (col % 26)));
        col /= 26;
    }
    return s;
}

void GroupedTableView::init(int rows, int cols, const QStringList& columnNames)
{
    // 1. Create and assign the standard item model
    auto *model = new QStandardItemModel(rows, cols, this);
    setModel(model);

    // 2. Title row headers with 1-based indices (1, 2, 3, ...)
    for (int r = 0; r < rows; ++r)
        model->setHeaderData(r, Qt::Vertical, QString::number(r + 1));

    // 3. Set column names (user-provided or fallback to Excel letters A, B, C...)
    for (int c = 0; c < cols; ++c) {
        QString name = (c < columnNames.size())
            ? columnNames[c]
            : columnLetter(c);

        model->setHeaderData(c, Qt::Horizontal, name);
    }

    // 4. Configure table view aesthetics and behaviors
    setShowGrid(true);
    setGridStyle(Qt::SolidLine);
    setAlternatingRowColors(true);
    setTabKeyNavigation(true);
    setFocusPolicy(Qt::StrongFocus);

    // 5. Configure header views (dimensions and alignments)
    verticalHeader()->setMinimumWidth(60);
    verticalHeader()->setDefaultAlignment(Qt::AlignRight | Qt::AlignVCenter);
    horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    horizontalHeader()->setMinimumHeight(56);
}

void GroupedTableView::fillTestData()
{
    auto *mdl = qobject_cast<QStandardItemModel*>(this->model());
    if (!mdl) return;

    for (int r = 0; r < mdl->rowCount(); ++r) {
        for (int c = 0; c < mdl->columnCount(); ++c) {
            mdl->setItem(r, c, new QStandardItem(
                QString("Cell %1%2").arg(columnLetter(c)).arg(r + 1)
            ));
        }
    }
}

void GroupedTableView::removeAllRowGroups()
{
    if (model()) {
        for (const auto &g : m_groupModel.rowGroups()) {
            for (int i = g.triggerRow + 1; i <= g.to; ++i)
                setRowHidden(i, false);
        }
    }
    m_vHeader->clearGroups();
    m_groupModel.clearRowGroups();
    viewport()->update();
    verticalHeader()->viewport()->update();
}

void GroupedTableView::removeAllColumnGroups()
{
    if (model()) {
        for (const auto &g : m_groupModel.columnGroups()) {
            for (int i = g.triggerCol + 1; i <= g.to; ++i)
                setColumnHidden(i, false);
        }
    }
    m_hHeader->clearGroups();
    m_groupModel.clearColumnGroups();
    viewport()->update();
    horizontalHeader()->viewport()->update();
}

void GroupedTableView::collapseAllRowGroups()
{
    for (const auto &g : m_groupModel.rowGroups()) {
        m_vHeader->setCollapsed(g.triggerRow, true);
    }
    viewport()->update();
    verticalHeader()->viewport()->update();
}

void GroupedTableView::expandAllRowGroups()
{
    for (const auto &g : m_groupModel.rowGroups()) {
        m_vHeader->setCollapsed(g.triggerRow, false);
    }
    viewport()->update();
    verticalHeader()->viewport()->update();
}

void GroupedTableView::collapseAllColumnGroups()
{
    for (const auto &g : m_groupModel.columnGroups()) {
        m_hHeader->setCollapsed(g.triggerCol, true);
    }
    viewport()->update();
    horizontalHeader()->viewport()->update();
}

void GroupedTableView::expandAllColumnGroups()
{
    for (const auto &g : m_groupModel.columnGroups()) {
        m_hHeader->setCollapsed(g.triggerCol, false);
    }
    viewport()->update();
    horizontalHeader()->viewport()->update();
}

QSize GroupedTableView::sizeHint() const
{
    int width = 0;
    int height = 0;

    // Sum up vertical header and visible columns
    if (verticalHeader()) {
        width += verticalHeader()->width();
    } else {
        width += 60;
    }

    const int cols = model() ? model()->columnCount() : 0;
    for (int c = 0; c < cols; ++c) {
        if (!isColumnHidden(c)) {
            width += columnWidth(c);
        }
    }

    // Sum up horizontal header and visible rows
    if (horizontalHeader()) {
        height += horizontalHeader()->height();
    } else {
        height += 56;
    }

    const int rows = model() ? model()->rowCount() : 0;
    for (int r = 0; r < rows; ++r) {
        if (!isRowHidden(r)) {
            height += rowHeight(r);
        }
    }

    const int frame = frameWidth() * 2;
    width += frame;
    height += frame;

    // Small extra padding to ensure scrollbars never flicker or show
    width += 4;
    height += 4;

    return QSize(width, height);
}

GroupedTableView* GroupedTableView::create(int rows, int cols, QWidget *parent)
{
    auto *table = new GroupedTableView(parent);
    table->init(rows, cols);
    return table;
}
