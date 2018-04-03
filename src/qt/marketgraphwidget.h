// Copyright (c) 2011-2013 The Bitcoin Core developers
// Copyright (c) 2016 The Hivemind Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
// Also see qcustomplot http://www.qcustomplot.com for information
// about the graphing library used by Hivemind-Qt.

#ifndef MARKETGRAPHWIDGET_H
#define MARKETGRAPHWIDGET_H

#include <QWidget>

class marketMarket;

namespace Ui {
class MarketGraphWidget;
}

class MarketGraphWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MarketGraphWidget(QWidget *parent = 0);
    ~MarketGraphWidget();

    /*
     * Market graph to be used inside of table cells.
     *
     * Should display a simple line X Y chart that is legible
     * at small sizes
     */
    QPixmap getTableGraphPixmap(QString title, const marketMarket *market);

    /*
     * Market graph to be used inside the TradeView widget.
     */
    void setupMarketTradeViewGraph(const marketMarket *market);

    /*
     * Popup market graph, full OHLC / candlestick chart
     * to be viewed in its own window.
     */
    void setupFullMarketGraph();

private:
    Ui::MarketGraphWidget *ui;

};

#endif // MARKETGRAPHWIDGET_H
