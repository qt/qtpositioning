// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QMetaType>
#include <QSignalSpy>
#include <QDebug>
#include <QTimer>

#include <QtPositioning/qgeosatelliteinfosource.h>
#include <QtPositioning/qgeosatelliteinfo.h>

#include "testqgeosatelliteinfosource_p.h"
#include "../utils/qlocationtestutils_p.h"

Q_DECLARE_METATYPE(QList<QGeoSatelliteInfo>)
Q_DECLARE_METATYPE(QGeoSatelliteInfoSource::Error)

#define MAX_WAITING_TIME 50000

// Must provide a valid source, unless testing the source
// returned by QGeoSatelliteInfoSource::createDefaultSource() on a system
// that has no default source
#define CHECK_SOURCE_VALID { \
        if (!m_source) { \
            if (m_testingDefaultSource && QGeoSatelliteInfoSource::createDefaultSource(0) == 0) \
                QSKIP("No default satellite source on this system"); \
            else \
                QFAIL("createTestSource() must return a valid source!"); \
        } \
    }

class MySatelliteSource : public QGeoSatelliteInfoSource
{
    Q_OBJECT
public:
    MySatelliteSource(QObject *parent = 0)
            : QGeoSatelliteInfoSource(parent) {
    }
    virtual void startUpdates() override {}
    virtual void stopUpdates() override {}
    virtual void requestUpdate(int) override {}
    virtual int minimumUpdateInterval() const override {
        return 0;
    }
    Error error() const override { return QGeoSatelliteInfoSource::NoError; }
};


class DefaultSourceTest : public TestQGeoSatelliteInfoSource
{
    Q_OBJECT
protected:
    QGeoSatelliteInfoSource *createTestSource() override {
        return QGeoSatelliteInfoSource::createDefaultSource(0);
    }
};

TestQGeoSatelliteInfoSource::TestQGeoSatelliteInfoSource(QObject *parent)
        : QObject(parent)
{
    qRegisterMetaType<QGeoSatelliteInfoSource::Error>();

    m_testingDefaultSource = false;
}

TestQGeoSatelliteInfoSource *TestQGeoSatelliteInfoSource::createDefaultSourceTest()
{
    DefaultSourceTest *test = new DefaultSourceTest;
    test->m_testingDefaultSource = true;
    return test;
}

void TestQGeoSatelliteInfoSource::base_initTestCase()
{
    qRegisterMetaType<QList<QGeoSatelliteInfo> >();
}

void TestQGeoSatelliteInfoSource::base_init()
{
    m_source = createTestSource();
    m_testSlot2Called = false;
}

void TestQGeoSatelliteInfoSource::base_cleanup()
{
    delete m_source;
    m_source = 0;
}

void TestQGeoSatelliteInfoSource::base_cleanupTestCase()
{
}

void TestQGeoSatelliteInfoSource::initTestCase()
{
    base_initTestCase();
}

void TestQGeoSatelliteInfoSource::init()
{
    base_init();
}

void TestQGeoSatelliteInfoSource::cleanup()
{
    base_cleanup();
}

void TestQGeoSatelliteInfoSource::cleanupTestCase()
{
    base_cleanupTestCase();
}

void TestQGeoSatelliteInfoSource::test_slot1()
{
}

void TestQGeoSatelliteInfoSource::test_slot2()
{
    m_testSlot2Called = true;
}

void TestQGeoSatelliteInfoSource::constructor_withParent()
{
    auto parent = std::make_unique<QObject>();
    new MySatelliteSource(parent.get());
}

void TestQGeoSatelliteInfoSource::constructor_noParent()
{
    MySatelliteSource *obj = new MySatelliteSource();
    delete obj;
}

void TestQGeoSatelliteInfoSource::createDefaultSource()
{
    auto parent = std::make_unique<QObject>();
    // source will be deleted by parent
    QGeoSatelliteInfoSource *source = QGeoSatelliteInfoSource::createDefaultSource(parent.get());

    // Check that default satellite source is successfully created.
    if (!QGeoSatelliteInfoSource::availableSources().isEmpty())
        QVERIFY(source);
    else
        QVERIFY(!source);
}

void TestQGeoSatelliteInfoSource::createDefaultSource_noParent()
{
    std::unique_ptr<QGeoSatelliteInfoSource> source(
            QGeoSatelliteInfoSource::createDefaultSource(0));

    // Check that default satellite source is successfully created.
    if (!QGeoSatelliteInfoSource::availableSources().isEmpty())
        QVERIFY(source);
    else
        QVERIFY(!source);
}

void TestQGeoSatelliteInfoSource::updateInterval()
{
    MySatelliteSource s;
    QCOMPARE(s.updateInterval(), 0);
}

void TestQGeoSatelliteInfoSource::setUpdateInterval()
{
    CHECK_SOURCE_VALID;

    QFETCH(int, interval);
    QFETCH(int, expectedInterval);

    m_source->setUpdateInterval(interval);
    QCOMPARE(m_source->updateInterval(), expectedInterval);
}

void TestQGeoSatelliteInfoSource::setUpdateInterval_data()
{
    QTest::addColumn<int>("interval");
    QTest::addColumn<int>("expectedInterval");
    QGeoSatelliteInfoSource *source = createTestSource();
    int minUpdateInterval = source ? source->minimumUpdateInterval() : -1;
    if (source)
        delete source;

    QTest::newRow("0") << 0 << 0;

    if (minUpdateInterval > -1) {
        QTest::newRow("INT_MIN") << INT_MIN << minUpdateInterval;
        QTest::newRow("-1") << -1 << minUpdateInterval;
    }

    if (minUpdateInterval > 0) {
        QTest::newRow("more than minInterval") << minUpdateInterval + 1 << minUpdateInterval + 1;
        QTest::newRow("equal to minInterval") << minUpdateInterval << minUpdateInterval;
    }

    if (minUpdateInterval > 1) {
        QTest::newRow("less then minInterval") << minUpdateInterval - 1 << minUpdateInterval;
        QTest::newRow("in btw zero and minInterval") << 1 << minUpdateInterval;
    }
}

void TestQGeoSatelliteInfoSource::minimumUpdateInterval()
{
    CHECK_SOURCE_VALID;

    QVERIFY(m_source->minimumUpdateInterval() > 0);
}

void TestQGeoSatelliteInfoSource::startUpdates_testIntervals()
{
    CHECK_SOURCE_VALID;
    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy errorSpy(m_source, SIGNAL(errorOccurred(QGeoSatelliteInfoSource::Error)));

    m_source->setUpdateInterval(1000);
    const int interval = 10000;

    m_source->startUpdates();

    if (!errorSpy.isEmpty())
        QSKIP("Error starting satellite updates.");

    QTRY_VERIFY_WITH_TIMEOUT((spyView.size() == 1) && (spyUse.size() == 1), interval);
    for (int i = 0; i < 6; i++) {
        QTRY_VERIFY_WITH_TIMEOUT((spyView.size() == 1) && (spyUse.size() == 1) && (errorSpy.size() == 0), interval);
        spyView.clear();
        spyUse.clear();
    }
    m_source->stopUpdates();
}


void TestQGeoSatelliteInfoSource::startUpdates_testIntervalChangesWhileRunning()
{
    // There are two ways of dealing with an interval change, and we have left it system dependent.
    // The interval can be changed will running or after the next update.
    // WinCE uses the first method, S60 uses the second method.

    // The minimum interval on the symbian emulator is 5000 msecs, which is why the times in
    // this test are as high as they are.

    CHECK_SOURCE_VALID;
    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy errorSpy(m_source, SIGNAL(errorOccurred(QGeoSatelliteInfoSource::Error)));

    m_source->setUpdateInterval(0);
    m_source->startUpdates();
    m_source->setUpdateInterval(0);

    if (!errorSpy.isEmpty())
        QSKIP("Error starting satellite updates.");

    QTRY_VERIFY_WITH_TIMEOUT((spyView.size() > 0) && (spyUse.size() > 0), 7000);
    QCOMPARE(errorSpy.size(), 0);
    spyView.clear();
    spyUse.clear();

    m_source->setUpdateInterval(1000);

    QTRY_VERIFY_WITH_TIMEOUT((spyView.size() == 2) && (spyUse.size() == 2) && (errorSpy.size() == 0), 15000);
    spyView.clear();
    spyUse.clear();

    m_source->setUpdateInterval(2000);

    QTRY_VERIFY_WITH_TIMEOUT((spyView.size() == 2) && (spyUse.size() == 2) && (errorSpy.size() == 0), 30000);
    spyView.clear();
    spyUse.clear();

    m_source->setUpdateInterval(1000);

    QTRY_VERIFY_WITH_TIMEOUT((spyView.size() == 2) && (spyUse.size() == 2) && (errorSpy.size() == 0), 15000);
    spyView.clear();
    spyUse.clear();

    m_source->setUpdateInterval(1000);

    QTRY_VERIFY_WITH_TIMEOUT( (spyView.size() == 2) && (spyUse.size() == 2) && (errorSpy.size() == 0), 15000);
    spyView.clear();
    spyUse.clear();

    m_source->setUpdateInterval(0);

    QTRY_VERIFY_WITH_TIMEOUT( (spyView.size() > 0 ) && (spyUse.size() > 0) && (errorSpy.size() == 0), 7000);
    spyView.clear();
    spyUse.clear();

    m_source->setUpdateInterval(0);

    QTRY_VERIFY_WITH_TIMEOUT( (spyView.size() > 0 ) && (spyUse.size() > 0) && (errorSpy.size() == 0), 7000);
    spyView.clear();
    spyUse.clear();
    m_source->stopUpdates();
}

void TestQGeoSatelliteInfoSource::startUpdates_testDefaultInterval()
{
    CHECK_SOURCE_VALID;
    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy errorSpy(m_source, SIGNAL(errorOccurred(QGeoSatelliteInfoSource::Error)));

    m_source->startUpdates();

    if (!errorSpy.isEmpty())
        QSKIP("Error starting satellite updates.");

    for (int i = 0; i < 3; i++) {
        QTRY_VERIFY_WITH_TIMEOUT( (spyView.size() > 0 ) && (spyUse.size() > 0) && (errorSpy.size() == 0), 7000);
        spyView.clear();
        spyUse.clear();
    }
    m_source->stopUpdates();
}

void TestQGeoSatelliteInfoSource::startUpdates_testZeroInterval()
{
    CHECK_SOURCE_VALID;
    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy errorSpy(m_source, SIGNAL(errorOccurred(QGeoSatelliteInfoSource::Error)));

    m_source->setUpdateInterval(0);
    m_source->startUpdates();

    if (!errorSpy.isEmpty())
        QSKIP("Error starting satellite updates.");

    for (int i = 0; i < 3; i++) {
        QTRY_VERIFY_WITH_TIMEOUT( (spyView.size() > 0 ) && (spyUse.size() > 0) && (errorSpy.size() == 0), 7000);
        spyView.clear();
        spyUse.clear();
    }
    m_source->stopUpdates();
}

void TestQGeoSatelliteInfoSource::startUpdates_moreThanOnce()
{
    CHECK_SOURCE_VALID;
    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy errorSpy(m_source, SIGNAL(errorOccurred(QGeoSatelliteInfoSource::Error)));

    m_source->setUpdateInterval(0);
    m_source->startUpdates();

    if (!errorSpy.isEmpty())
        QSKIP("Error starting satellite updates.");

    m_source->startUpdates(); // check there is no crash

    QTRY_VERIFY_WITH_TIMEOUT((spyView.size() > 0) && (spyUse.size() > 0), MAX_WAITING_TIME);

    m_source->startUpdates(); // check there is no crash

    m_source->stopUpdates();
}

void TestQGeoSatelliteInfoSource::stopUpdates()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy errorSpy(m_source, SIGNAL(errorOccurred(QGeoSatelliteInfoSource::Error)));

    m_source->setUpdateInterval(1000);
    m_source->startUpdates();

    if (!errorSpy.isEmpty())
        QSKIP("Error starting satellite updates.");

    for (int i = 0; i < 2; i++) {
        QTRY_VERIFY_WITH_TIMEOUT((spyView.size() == 1) && (spyUse.size() == 1), 12000);
        spyView.clear();
        spyUse.clear();
    }

    m_source->stopUpdates();

    QTRY_VERIFY_WITH_TIMEOUT((spyView.size() == 0) && (spyUse.size() == 0), 12000);
}

void TestQGeoSatelliteInfoSource::stopUpdates_withoutStart()
{
    CHECK_SOURCE_VALID;

    m_source->stopUpdates(); // check there is no crash
}

void TestQGeoSatelliteInfoSource::requestUpdate()
{
    CHECK_SOURCE_VALID;

    QFETCH(int, timeout);
    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy errorSpy(m_source, SIGNAL(errorOccurred(QGeoSatelliteInfoSource::Error)));

    m_source->requestUpdate(timeout);

    if (!errorSpy.isEmpty()) {
        // make sure that it's the UpdateTimeoutError
        const auto error = errorSpy[0][0].value<QGeoSatelliteInfoSource::Error>();
        QCOMPARE(error, QGeoSatelliteInfoSource::UpdateTimeoutError);
        // errorSpy.clear(); // no need to clear the error, as we are expecting it
    }

    // Geoclue may deliver update instantly if there is a satellite fix
    QTRY_VERIFY_WITH_TIMEOUT(!errorSpy.isEmpty() || !spyView.isEmpty(), 10);
    if (!errorSpy.isEmpty()) {
        // make sure that it's the UpdateTimeoutError
        const auto error = errorSpy[0][0].value<QGeoSatelliteInfoSource::Error>();
        QCOMPARE(error, QGeoSatelliteInfoSource::UpdateTimeoutError);
    }
}

void TestQGeoSatelliteInfoSource::requestUpdate_data()
{
    QTest::addColumn<int>("timeout");
    QTest::newRow("less than zero") << -1;
    QTest::newRow("very small timeout") << 1;
}

void TestQGeoSatelliteInfoSource::requestUpdate_validTimeout()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy errorSpy(m_source, SIGNAL(errorOccurred(QGeoSatelliteInfoSource::Error)));

    m_source->requestUpdate(1500);

    if (!errorSpy.isEmpty())
        QSKIP("Error starting satellite updates.");

    QTRY_VERIFY_WITH_TIMEOUT(
            (spyView.size() == 1) && (spyUse.size() == 1 && (errorSpy.size()) == 0), 7000);
}

void TestQGeoSatelliteInfoSource::requestUpdate_defaultTimeout()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy errorSpy(m_source, SIGNAL(errorOccurred(QGeoSatelliteInfoSource::Error)));

    m_source->requestUpdate(0);

    if (!errorSpy.isEmpty())
        QSKIP("Error starting satellite updates.");

    QTRY_VERIFY_WITH_TIMEOUT(
            (spyView.size() == 1) && (spyUse.size() == 1 && (errorSpy.size()) == 0),
            MAX_WAITING_TIME);
}

void TestQGeoSatelliteInfoSource::requestUpdate_timeoutLessThanMinimumInterval()
{
    CHECK_SOURCE_VALID;

    QSignalSpy errorSpy(m_source, SIGNAL(errorOccurred(QGeoSatelliteInfoSource::Error)));

    m_source->requestUpdate(1);

    QTRY_COMPARE_WITH_TIMEOUT(errorSpy.size(), 1, 1000);
    const auto error = errorSpy[0][0].value<QGeoSatelliteInfoSource::Error>();
    QCOMPARE(error, QGeoSatelliteInfoSource::UpdateTimeoutError);
}

void TestQGeoSatelliteInfoSource::requestUpdate_repeatedCalls()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy errorSpy(m_source, SIGNAL(errorOccurred(QGeoSatelliteInfoSource::Error)));

    m_source->requestUpdate(1500);

    if (!errorSpy.isEmpty())
        QSKIP("Error starting satellite updates.");

    QTRY_VERIFY_WITH_TIMEOUT((spyView.size() == 1) && (spyUse.size() == 1), 7000);
    spyView.clear();
    spyUse.clear();

    m_source->requestUpdate(1500);

    QTRY_VERIFY_WITH_TIMEOUT((spyView.size() == 1) && (spyUse.size() == 1), 7000);
}

void TestQGeoSatelliteInfoSource::requestUpdate_overlappingCalls()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy errorSpy(m_source, SIGNAL(errorOccurred(QGeoSatelliteInfoSource::Error)));

    m_source->requestUpdate(1500);

    if (!errorSpy.isEmpty())
        QSKIP("Error starting satellite updates.");

    m_source->requestUpdate(1500);

    QTRY_VERIFY_WITH_TIMEOUT((spyView.size() == 1) && (spyUse.size() == 1), 7000);
}

void TestQGeoSatelliteInfoSource::requestUpdate_overlappingCallsWithTimeout()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy errorSpy(m_source, SIGNAL(errorOccurred(QGeoSatelliteInfoSource::Error)));

    m_source->requestUpdate(0);

    if (!errorSpy.isEmpty())
        QSKIP("Error starting satellite updates.");

    m_source->requestUpdate(1);

    QTRY_COMPARE_WITH_TIMEOUT(errorSpy.size(), 0, 7000);

    QTRY_VERIFY_WITH_TIMEOUT((spyView.size() == 1) && (spyUse.size() == 1), 7000);
}

void TestQGeoSatelliteInfoSource::requestUpdateAfterStartUpdates_ZeroInterval()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy errorSpy(m_source, SIGNAL(errorOccurred(QGeoSatelliteInfoSource::Error)));

    m_source->setUpdateInterval(0);
    m_source->startUpdates();

    if (!errorSpy.isEmpty())
        QSKIP("Error starting satellite updates.");

    QTRY_VERIFY_WITH_TIMEOUT((spyView.size() >= 1) && (spyUse.size() >= 1), MAX_WAITING_TIME);
    spyView.clear();
    spyUse.clear();

    m_source->requestUpdate(1500);

    QTRY_VERIFY_WITH_TIMEOUT((spyView.size() >= 1) && (spyUse.size() >= 1)
                             && (errorSpy.size() == 0), 7000);

    spyView.clear();
    spyUse.clear();

    QTRY_VERIFY_WITH_TIMEOUT((spyView.size() >= 1) && (spyUse.size() >= 1), 12000);

    m_source->stopUpdates();
}

void TestQGeoSatelliteInfoSource::requestUpdateAfterStartUpdates_SmallInterval()
{
    CHECK_SOURCE_VALID;

    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy errorSpy(m_source, SIGNAL(errorOccurred(QGeoSatelliteInfoSource::Error)));

    m_source->setUpdateInterval(3000);
    m_source->requestUpdate(1500);

    if (!errorSpy.isEmpty())
        QSKIP("Error starting satellite updates.");

    m_source->startUpdates();

    QTRY_VERIFY_WITH_TIMEOUT((spyView.size() > 0) && (spyUse.size() > 0)
                             && (errorSpy.size() == 0), 7000);

    spyView.clear();
    spyUse.clear();

    QTRY_VERIFY_WITH_TIMEOUT((spyView.size() == 1) && (spyUse.size() == 1), 12000);

    m_source->stopUpdates();
}

void TestQGeoSatelliteInfoSource::requestUpdateBeforeStartUpdates_ZeroInterval()
{
    CHECK_SOURCE_VALID;
    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy errorSpy(m_source, SIGNAL(errorOccurred(QGeoSatelliteInfoSource::Error)));

    m_source->requestUpdate(1500);

    if (!errorSpy.isEmpty())
        QSKIP("Error starting satellite updates.");

    m_source->setUpdateInterval(0);
    m_source->startUpdates();

    QTRY_VERIFY_WITH_TIMEOUT((spyView.size() >= 2) && (spyUse.size() >= 2) && (errorSpy.size() == 0), 14000);
    spyView.clear();
    spyUse.clear();

    QTest::qWait(1500);

    QCOMPARE(errorSpy.size(), 0);

    m_source->stopUpdates();
}

void TestQGeoSatelliteInfoSource::requestUpdateBeforeStartUpdates_SmallInterval()
{
    CHECK_SOURCE_VALID;
    QSignalSpy spyView(m_source,
                       SIGNAL(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy spyUse(m_source,
                      SIGNAL(satellitesInUseUpdated(QList<QGeoSatelliteInfo>)));
    QSignalSpy errorSpy(m_source, SIGNAL(errorOccurred(QGeoSatelliteInfoSource::Error)));

    m_source->requestUpdate(1500);

    if (!errorSpy.isEmpty())
        QSKIP("Error starting satellite updates.");

    m_source->setUpdateInterval(2000);
    m_source->startUpdates();

    QTRY_VERIFY_WITH_TIMEOUT((spyView.size() > 0) && (spyUse.size() > 0) && (errorSpy.size() == 0), 7000);
    spyView.clear();
    spyUse.clear();

    QTRY_VERIFY_WITH_TIMEOUT((spyView.size() > 0) && (spyUse.size() > 0) && (errorSpy.size() == 0), 20000);

    m_source->stopUpdates();
}



void TestQGeoSatelliteInfoSource::removeSlotForRequestTimeout()
{
    CHECK_SOURCE_VALID;

    bool i = connect(m_source, SIGNAL(errorOccurred(QGeoSatelliteInfoSource::Error)),
                     this, SLOT(test_slot1()));
    QVERIFY(i==true);
    i = connect(m_source, SIGNAL(errorOccurred(QGeoSatelliteInfoSource::Error)), \
                this, SLOT(test_slot2()));
    QVERIFY(i==true);
    i = disconnect(m_source, SIGNAL(errorOccurred(QGeoSatelliteInfoSource::Error)),
                   this, SLOT(test_slot1()));
    QVERIFY(i==true);

    m_source->requestUpdate(-1);
    QTRY_VERIFY_WITH_TIMEOUT((m_testSlot2Called == true), 1000);
}

void TestQGeoSatelliteInfoSource::removeSlotForSatellitesInUseUpdated()
{
    CHECK_SOURCE_VALID;

    bool i = connect(m_source, SIGNAL(satellitesInUseUpdated(QList<QGeoSatelliteInfo>)), this, SLOT(test_slot1()));
    QVERIFY(i == true);
    i = connect(m_source, SIGNAL(satellitesInUseUpdated(QList<QGeoSatelliteInfo>)), this, SLOT(test_slot2()));
    QVERIFY(i == true);
    i = disconnect(m_source, SIGNAL(satellitesInUseUpdated(QList<QGeoSatelliteInfo>)), this, SLOT(test_slot1()));
    QVERIFY(i == true);

    m_source->requestUpdate(1500);

    if (m_source->error() != QGeoSatelliteInfoSource::NoError)
        QSKIP("Error starting satellite updates.");

    QTRY_VERIFY_WITH_TIMEOUT((m_testSlot2Called == true), 7000);
}

void TestQGeoSatelliteInfoSource::removeSlotForSatellitesInViewUpdated()
{
    CHECK_SOURCE_VALID;

    bool i = connect(m_source, SIGNAL(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)), this, SLOT(test_slot1()));
    QVERIFY(i == true);
    i = connect(m_source, SIGNAL(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)), this, SLOT(test_slot2()));
    QVERIFY(i == true);
    i = disconnect(m_source, SIGNAL(satellitesInViewUpdated(QList<QGeoSatelliteInfo>)), this, SLOT(test_slot1()));
    QVERIFY(i == true);

    m_source->requestUpdate(1500);

    if (m_source->error() != QGeoSatelliteInfoSource::NoError)
        QSKIP("Error starting satellite updates.");

    QTRY_VERIFY_WITH_TIMEOUT((m_testSlot2Called == true), 7000);
}

void TestQGeoSatelliteInfoSource::bindings()
{
    CHECK_SOURCE_VALID;

    QTestPrivate::testReadWritePropertyBasics<QGeoSatelliteInfoSource, int>(
            *m_source, 1000, 2000, "updateInterval",
            []() {
                return std::unique_ptr<QGeoSatelliteInfoSource>(
                        QGeoSatelliteInfoSource::createDefaultSource(nullptr));
            });
}

#include "testqgeosatelliteinfosource.moc"
