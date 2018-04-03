// Copyright (c) 2015 The Hivemind Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HIVEMIND_QT_DECISIONVIEWGRAPH_H
#define HIVEMIND_QT_DECISIONVIEWGRAPH_H

#include <stdint.h>
#include <QWidget>


class DecisionViewGraph
   : public QWidget
{
    Q_OBJECT

public:
    explicit DecisionViewGraph(QWidget *parent=0);
    void paintEvent(QPaintEvent *);
    void setData(const double *X, const double *Y, uint32_t N);

private:
    double *X;  /* X[] */
    double *Y;  /* Y[] */
    uint32_t N;
    bool dataIsChanging;
};

#endif // HIVEMIND_QT_DECISIONVIEWGRAPH_H
