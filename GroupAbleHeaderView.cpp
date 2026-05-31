#include "GroupAbleHeaderView.h"
#include "GroupedTableView.h"
#include <QPainter>
#include <QMouseEvent>
#include <QStyleOptionHeader>

GroupableHeaderView::GroupableHeaderView(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent)
{
    setSectionsClickable(true);
    setDefaultAlignment(Qt::AlignCenter);
}

void GroupableHeaderView::addGroup(int logicalIndex)
{
    if (!m_groups.contains(logicalIndex))
        m_groups.push_back(logicalIndex);

    updateSection(logicalIndex);
}

void GroupableHeaderView::removeGroup(int logicalIndex)
{
    m_groups.removeAll(logicalIndex); 
    m_collapsed.remove(logicalIndex);

    updateSection(logicalIndex);
}

bool GroupableHeaderView::hasGroup(int logicalIndex) const
{
    return m_groups.contains(logicalIndex);
}

void GroupableHeaderView::setCollapsed(int logicalIndex, bool collapsed)
{
    if (!hasGroup(logicalIndex)) return;

    if (m_collapsed.value(logicalIndex, false) == collapsed)
        return;

    m_collapsed[logicalIndex] = collapsed;
    updateSection(logicalIndex);

    viewport()->update();

    emit groupToggled(logicalIndex, collapsed);
}

bool GroupableHeaderView::isCollapsed(int logicalIndex) const
{
    return m_collapsed.value(logicalIndex, false);
}

void GroupableHeaderView::clearGroups()
{
    m_groups.clear();
    m_collapsed.clear();

    viewport()->update();
}

QRect GroupableHeaderView::buttonRect(const QRect &sectionRect) const
{
    const int bs = buttonSize();
    if (orientation() == Qt::Vertical) {
        return QRect(sectionRect.left() + 6,
                     sectionRect.top() + (sectionRect.height() - bs) / 2,
                     bs, bs);
    } else {
        return QRect(sectionRect.left() + (sectionRect.width() - bs) / 2,
                     sectionRect.top() + 3,
                     bs, bs);
    }
}

void GroupableHeaderView::drawGroupButton(QPainter *painter, const QRect &btnRect, bool collapsed) const
{
    painter->save();
    // Disable antialiasing for crisp pixel-aligned 1px borders and lines
    painter->setRenderHint(QPainter::Antialiasing, false);

    const bool isDark = (palette().text().color().lightness() > 128);
    
    // Modern soft-grey colors that adapt to theme
    QColor btnBg = isDark ? QColor(60, 60, 60) : QColor(248, 248, 248);
    QColor btnBorder = isDark ? QColor(110, 110, 110) : QColor(180, 180, 180);
    QColor symbolColor = isDark ? QColor(230, 230, 230) : QColor(70, 70, 70);

    // Draw square button background and border
    painter->setPen(QPen(btnBorder, 1));
    painter->setBrush(QBrush(btnBg));
    painter->drawRect(btnRect);

    // Calculate center coordinate
    const int cx = btnRect.left() + btnRect.width() / 2;
    const int cy = btnRect.top() + btnRect.height() / 2;

    // Draw plus/minus sign using crisp lines
    painter->setPen(QPen(symbolColor, 1, Qt::SolidLine, Qt::FlatCap));
    
    // Horizontal line
    painter->drawLine(cx - 3, cy, cx + 3, cy);

    // Vertical line (if collapsed to show '+')
    if (collapsed) {
        painter->drawLine(cx, cy - 3, cx, cy + 3);
    }

    painter->restore();
}

void GroupableHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    // === Horizontal Header ===
    if (orientation() == Qt::Horizontal) {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        // 1. Standard background and frame
        QStyleOptionHeader opt;
        initStyleOption(&opt);
        opt.rect = rect;
        opt.section = logicalIndex;
        opt.text.clear(); // Disable default text rendering
        style()->drawControl(QStyle::CE_Header, &opt, painter, this);

        // 2. Group button (if present)
        if (hasGroup(logicalIndex)) {
            const QRect btnRect = buttonRect(rect);
            drawGroupButton(painter, btnRect, isCollapsed(logicalIndex));
        }

        // 3. Column title text in lower area
        const QColor textColor = palette().buttonText().color();
        painter->setPen(textColor);
        const QFont textFont("Segoe UI", 9);
        painter->setFont(textFont);

        const QRect textZone = rect.adjusted(4, 28, -4, -4);
        if (model()) {
            const QString text = model()->headerData(logicalIndex, Qt::Horizontal).toString();
            painter->drawText(textZone, Qt::AlignCenter | Qt::AlignBottom, text);
        }

        painter->restore();
        return;
    }

    // === Vertical Header (rows) ===
    if (!hasGroup(logicalIndex)) {
        QHeaderView::paintSection(painter, rect, logicalIndex);
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    QStyleOptionHeader opt;
    initStyleOption(&opt);
    opt.rect = rect;
    opt.section = logicalIndex;
    opt.text.clear(); // Disable default text rendering
    style()->drawControl(QStyle::CE_Header, &opt, painter, this);

    const QRect btnRect = buttonRect(rect);
    drawGroupButton(painter, btnRect, isCollapsed(logicalIndex));

    // Row number on the right
    const QColor textColor = palette().buttonText().color();
    painter->setPen(textColor);
    const QFont textFont("Segoe UI", 9);
    painter->setFont(textFont);

    const QRect textZone = rect.adjusted(32, 0, -6, 0);
    if (model()) {
        const QString text = model()->headerData(logicalIndex, Qt::Vertical).toString();
        painter->drawText(textZone, Qt::AlignRight | Qt::AlignVCenter, text);
    }

    painter->restore();
}

void GroupableHeaderView::mousePressEvent(QMouseEvent *event)
{
    const QPoint pos = event->pos();
    const int logicalIndex = logicalIndexAt(pos);

    if (logicalIndex == -1 || !hasGroup(logicalIndex)) {
        QHeaderView::mousePressEvent(event);
        return;
    }

    const int sectionPos = sectionViewportPosition(logicalIndex);
    const QRect sectionRect = (orientation() == Qt::Horizontal)
                                  ? QRect(sectionPos, 0, sectionSize(logicalIndex), height())
                                  : QRect(0, sectionPos, width(), sectionSize(logicalIndex));

    if (buttonRect(sectionRect).contains(pos)) {
        const bool newState = !isCollapsed(logicalIndex);
        setCollapsed(logicalIndex, newState);
        return;
    }

    QHeaderView::mousePressEvent(event);
}

QSize GroupableHeaderView::sectionSizeFromContents(int logicalIndex) const
{
    QSize size = QHeaderView::sectionSizeFromContents(logicalIndex);
    if (hasGroup(logicalIndex)) {
        if (orientation() == Qt::Vertical)
            size.setWidth(qMax(size.width(), 52));
        else
            size.setHeight(size.height() + 32);
    }
    return size;
}

void GroupableHeaderView::paintEvent(QPaintEvent *event)
{
    // 1. Standard painting
    QHeaderView::paintEvent(event);

    // 2. Custom painting
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::darkGray, 1));

    // 3. Draw lines for all expanded groups
    for (int trigger : m_groups) {
        if (!isCollapsed(trigger)) {
            if (orientation() == Qt::Vertical)
                drawRowGroupLine(&painter, trigger);
            else
                drawColumnGroupLine(&painter, trigger);
        }
    }
}

void GroupableHeaderView::drawRowGroupLine(QPainter *p, int trigger)
{
    auto *table = qobject_cast<GroupedTableView*>(parent());
    if (!table) return;

    const auto &groups = table->rowGroups();
    for (const auto &g : groups) {
        if (g.triggerRow != trigger)
            continue;

        // Vertical rail is drawn at the horizontal center of the toggle button.
        // Button starts at x=6 and is buttonSize() wide.
        const int railX  = 6 + buttonSize() / 2;
        const int legLen = 12;   // length of the horizontal turn at the bottom
        const int radius = 3;

        // Top of the rail: just below the button center (bottom half of trigger section)
        const int startY = sectionViewportPosition(trigger) + sectionSize(trigger) / 2 + buttonSize() / 2 + 2;
        // Bottom of the rail: vertical centre of the last grouped section
        const int endY   = sectionViewportPosition(g.to) + sectionSize(g.to) / 2;

        // 1. Long vertical rail from the trigger button downward
        p->drawLine(railX, startY, railX, endY);

        // 2. Short horizontal turn to the right at the bottom
        p->drawLine(railX, endY, railX + legLen - radius, endY);

        // 3. Terminal circle
        p->setBrush(Qt::NoBrush);
        p->setPen(QPen(Qt::darkGray, 1));
        p->drawEllipse(QPoint(railX + legLen, endY), radius, radius);
    }
}

void GroupableHeaderView::drawColumnGroupLine(QPainter *p, int trigger)
{
    auto *table = qobject_cast<GroupedTableView*>(parent());
    if (!table) return;

    const auto &groups = table->columnGroups();

    for (const auto &g : groups) {
        if (g.triggerCol != trigger)
            continue;

        const int startX = sectionViewportPosition(trigger) + sectionSize(trigger) / 2;
        const int endX   = sectionViewportPosition(g.to) + sectionSize(g.to) / 2;

        const int bs = buttonSize();
        const int y = 3 + bs / 2;
        const int radius = 3;
        const int leg = 12;

        // Horizontal line starting 3px to the right of the button's right edge
        p->drawLine(startX + bs / 2 + 3, y, endX, y);

        // Vertical "leg" down (shortened, does not enter the circle)
        p->drawLine(endX, y, endX, y + leg - radius);

        p->setBrush(Qt::NoBrush);
        p->setPen(QPen(Qt::darkGray, 1));
        p->drawEllipse(QPoint(endX, y + leg), radius, radius);
    }
}
