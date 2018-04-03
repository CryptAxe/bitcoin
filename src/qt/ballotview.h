// Copyright (c) 2015 The Hivemind Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HIVEMIND_QT_BALLOTVIEW_H
#define HIVEMIND_QT_BALLOTVIEW_H


#include <vector>
#include <QLabel>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QGridLayout;
class QLineEdit;
QT_END_NAMESPACE

#include "guiutil.h"

class marketBranch;
class marketDecision;
class marketOutcome;
class marketRevealVote;
class marketSealedVote;
class BallotBallotWindow;
class BallotBranchWindow;
class BallotOutcomeWindow;
class BallotSealedVoteWindow;
class BallotVoteWindow;
class WalletModel;


#define BALLOTBRANCH_NLABLES       15
#define BALLOTBALLOT_NLABLES        4
#define BALLOTOUTCOME_NLABLES       2
#define BALLOTSEALEDVOTE_NLABLES   13
#define BALLOTVOTE_NLABLES         13
#define BALLOT_NBALLOTGRIDLAYOUTS   1
#define BALLOT_NOUTCOMEGRIDLAYOUTS  6


class BallotView
    : public QWidget
{
    Q_OBJECT

public:
    explicit BallotView(QWidget *parent=0);

    void setModel(WalletModel *model);
    void onBallotChange(unsigned int);
    void onBranchChange(const marketBranch *);
    void onOutcomeChange(const marketOutcome *);
    void onSealedVoteChange(const marketSealedVote *);
    void onVoteChange(const marketRevealVote *);

private:
    void initSelectBranchTab(QWidget *);
    void initSelectBallotTab(QWidget *);
    void initSelectSealedVoteTab(QWidget *);
    void initSelectVoteTab(QWidget *);
    void initSelectOutcomeTab(QWidget *);
    void initCreateSealedVoteTab(QWidget *);
    void initCreateVoteTab(QWidget *);
    void updateCreateSealedVoteCLI(void);
    void updateCreateSealedVoteScrollArea(void);
    void updateCreateVoteCLI(void);

private:
    WalletModel *model;

    QLabel *branchLabels[2];
    QLabel *ballotLabels[2];

    /* select tab variables */
    BallotBranchWindow *branchWindow;
    const marketBranch *branch;
    QLabel branchTabLabels[BALLOTBRANCH_NLABLES];

    BallotBallotWindow *ballotWindow;
    uint32_t ballotNum;
    QLabel ballotTabLabels[BALLOTBALLOT_NLABLES];
    QGridLayout *ballotLayouts[BALLOT_NBALLOTGRIDLAYOUTS];

    BallotOutcomeWindow *outcomeWindow;
    const marketOutcome *outcome;
    QLabel outcomeTabLabels[BALLOTOUTCOME_NLABLES];
    QGridLayout *outcomeLayouts[BALLOT_NOUTCOMEGRIDLAYOUTS];

    BallotSealedVoteWindow *sealedVoteWindow;
    const marketSealedVote *sealedVote;
    QLabel sealedVoteTabLabels[BALLOTSEALEDVOTE_NLABLES];
    std::vector<marketDecision *> sealedVoteDecisions;
    std::vector<double> sealedVoteVotes;

    BallotVoteWindow *voteWindow;
    const marketRevealVote *vote;
    QLabel voteTabLabels[BALLOTVOTE_NLABLES];

    /* create tab variables */
    QLabel *sealedVoteBranchLabel;
    QLineEdit *sealedVoteHeight;
    QLineEdit *sealedVoteAddress;
    QGridLayout *sealedVoteLayout;
    QLabel *createSealedVoteCLI;
    QLabel *createSealedVoteCLIResponse;

    QLabel *voteBranchLabel;
    QLineEdit *voteHeight;
    QLineEdit *voteAddress;
    QLabel *createVoteCLI;
    QLabel *createVoteCLIResponse;

public Q_SLOTS:
    void onSealedVoteHeightTextChanged(const QString &);
    void onSealedVoteAddressTextChanged(const QString &);
    void onVoteHeightTextChanged(const QString &);
    void onVoteAddressTextChanged(const QString &);
    void onCreateSealedVoteClicked(void);
    void onCreateVoteClicked(void);
    void showBallotWindow(void);
    void showBranchWindow(void);
    void showOutcomeWindow(void);
    void showSealedVoteWindow(void);
    void showVoteWindow(void);
};

#endif // HIVEMIND_QT_BALLOTVIEW_H
