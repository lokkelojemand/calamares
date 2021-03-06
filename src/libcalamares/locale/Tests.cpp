/* === This file is part of Calamares - <https://github.com/calamares> ===
 *
 *   SPDX-FileCopyrightText: 2019 Adriaan de Groot <groot@kde.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 *   Calamares is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Calamares is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Calamares. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "locale/LabelModel.h"
#include "locale/TimeZone.h"
#include "locale/TranslatableConfiguration.h"

#include "CalamaresVersion.h"
#include "utils/Logger.h"

#include <QtTest/QtTest>

class LocaleTests : public QObject
{
    Q_OBJECT
public:
    LocaleTests();
    ~LocaleTests() override;

private Q_SLOTS:
    void initTestCase();

    void testLanguageModelCount();
    void testTranslatableLanguages();
    void testTranslatableConfig1();
    void testTranslatableConfig2();
    void testLanguageScripts();

    void testEsperanto();
    void testInterlingue();

    // TimeZone testing
    void testRegions();
    void testSimpleZones();
    void testComplexZones();
    void testTZLookup();
    void testTZIterator();
    void testLocationLookup_data();
    void testLocationLookup();
    void testLocationLookup2();
};

LocaleTests::LocaleTests() {}

LocaleTests::~LocaleTests() {}

void
LocaleTests::initTestCase()
{
    Logger::setupLogLevel( Logger::LOGDEBUG );

    // Otherwise plain get() is dubious in the TranslatableConfiguration tests
    QLocale::setDefault( QLocale( QStringLiteral( "en_US" ) ) );
    QVERIFY( ( QLocale().name() == "C" ) || ( QLocale().name() == "en_US" ) );
}

void
LocaleTests::testLanguageModelCount()
{
    const auto* m = CalamaresUtils::Locale::availableTranslations();

    QVERIFY( m );
    QVERIFY( m->rowCount( QModelIndex() ) > 1 );

    int dutch = m->find( QLocale( "nl_NL" ) );
    QVERIFY( dutch > 0 );
    QCOMPARE( m->find( "NL" ), dutch );
    // must be capitals
    QCOMPARE( m->find( "nl" ), -1 );
    QCOMPARE( m->find( QLocale( "nl" ) ), dutch );

    // Belgium speaks Dutch as well
    QCOMPARE( m->find( "BE" ), dutch );
}

void
LocaleTests::testLanguageScripts()
{
    const auto* m = CalamaresUtils::Locale::availableTranslations();

    QVERIFY( m );

    // Cursory test that all the locales found have a sensible language,
    // and that some specific languages have sensible corresponding data.
    //
    // This fails on Esperanto (or, if Esperanto is added to Qt, then
    // this will pass and the test after the loop will fail.
    for ( int i = 0; i < m->rowCount( QModelIndex() ); ++i )
    {
        const auto& label = m->locale( i );
        const auto locale = label.locale();
        cDebug() << label.label() << locale;

        QVERIFY( locale.language() == QLocale::Greek ? locale.script() == QLocale::GreekScript : true );
        QVERIFY( locale.language() == QLocale::Korean ? locale.script() == QLocale::KoreanScript : true );
        QVERIFY( locale.language() == QLocale::Lithuanian ? locale.country() == QLocale::Lithuania : true );
        QVERIFY( locale.language() != QLocale::C );
    }
}

void
LocaleTests::testEsperanto()
{
#if QT_VERSION < QT_VERSION_CHECK( 5, 12, 2 )
    QCOMPARE( QLocale( "eo" ).language(), QLocale::C );
    QCOMPARE( QLocale( QLocale::Esperanto ).language(), QLocale::English );
#else
    QCOMPARE( QLocale( "eo" ).language(), QLocale::Esperanto );
    QCOMPARE( QLocale( QLocale::Esperanto ).language(), QLocale::Esperanto );  // Probably fails on 5.12, too
#endif
}

void
LocaleTests::testInterlingue()
{
    // ie / Interlingue is borked (is "ie" even the right name?)
    QCOMPARE( QLocale( "ie" ).language(), QLocale::C );
    QCOMPARE( QLocale( QLocale::Interlingue ).language(), QLocale::English );

    // "ia" exists (post-war variant of Interlingue)
    QCOMPARE( QLocale( "ia" ).language(), QLocale::Interlingua );
    // "bork" does not exist
    QCOMPARE( QLocale( "bork" ).language(), QLocale::C );
}


static const QStringList&
someLanguages()
{
    static QStringList languages { "nl", "de", "da", "nb", "sr@latin", "ar", "ru" };
    return languages;
}


/** @brief Check consistency of test data
 * Check that all the languages used in testing, are actually enabled
 * in Calamares translations.
 */
void
LocaleTests::testTranslatableLanguages()
{
    QStringList availableLanguages = QString( CALAMARES_TRANSLATION_LANGUAGES ).split( ';' );
    cDebug() << "Translation languages:" << availableLanguages;
    for ( const auto& language : someLanguages() )
    {
        // Could be QVERIFY, but then we don't see what language code fails
        QCOMPARE( availableLanguages.contains( language ) ? language : QString(), language );
    }
}

/** @brief Test strings with no translations
 */
void
LocaleTests::testTranslatableConfig1()
{
    CalamaresUtils::Locale::TranslatedString ts0;
    QVERIFY( ts0.isEmpty() );
    QCOMPARE( ts0.count(), 1 );  // the empty string

    CalamaresUtils::Locale::TranslatedString ts1( "Hello" );
    QCOMPARE( ts1.count(), 1 );
    QVERIFY( !ts1.isEmpty() );

    QCOMPARE( ts1.get(), QStringLiteral( "Hello" ) );
    QCOMPARE( ts1.get( QLocale( "nl" ) ), QStringLiteral( "Hello" ) );

    QVariantMap map;
    map.insert( "description", "description (no language)" );
    CalamaresUtils::Locale::TranslatedString ts2( map, "description" );
    QCOMPARE( ts2.count(), 1 );
    QVERIFY( !ts2.isEmpty() );

    QCOMPARE( ts2.get(), QStringLiteral( "description (no language)" ) );
    QCOMPARE( ts2.get( QLocale( "nl" ) ), QStringLiteral( "description (no language)" ) );
}

/** @bref Test strings with translations.
 */
void
LocaleTests::testTranslatableConfig2()
{
    QVariantMap map;

    for ( const auto& language : someLanguages() )
    {
        map.insert( QString( "description[%1]" ).arg( language ),
                    QString( "description (language %1)" ).arg( language ) );
        if ( language != "nl" )
        {
            map.insert( QString( "name[%1]" ).arg( language ), QString( "name (language %1)" ).arg( language ) );
        }
    }

    // If there's no untranslated string in the map, it is considered empty
    CalamaresUtils::Locale::TranslatedString ts0( map, "description" );
    QVERIFY( ts0.isEmpty() );  // Because no untranslated string
    QCOMPARE( ts0.count(),
              someLanguages().count() + 1 );  // But there are entries for the translations, plus an empty string

    // expand the map with untranslated entries
    map.insert( QString( "description" ), "description (no language)" );
    map.insert( QString( "name" ), "name (no language)" );

    CalamaresUtils::Locale::TranslatedString ts1( map, "description" );
    // The +1 is because "" is always also inserted
    QCOMPARE( ts1.count(), someLanguages().count() + 1 );
    QVERIFY( !ts1.isEmpty() );

    QCOMPARE( ts1.get(), QStringLiteral( "description (no language)" ) );  // it wasn't set
    QCOMPARE( ts1.get( QLocale( "nl" ) ), QStringLiteral( "description (language nl)" ) );
    for ( const auto& language : someLanguages() )
    {
        // Skip Serbian (latin) because QLocale() constructed with it
        // doesn't retain the @latin part.
        if ( language == "sr@latin" )
        {
            continue;
        }
        // Could be QVERIFY, but then we don't see what language code fails
        QCOMPARE( ts1.get( language ) == QString( "description (language %1)" ).arg( language ) ? language : QString(),
                  language );
    }
    QCOMPARE( ts1.get( QLocale( QLocale::Language::Serbian, QLocale::Script::LatinScript, QLocale::Country::Serbia ) ),
              QStringLiteral( "description (language sr@latin)" ) );

    CalamaresUtils::Locale::TranslatedString ts2( map, "name" );
    // We skipped dutch this time
    QCOMPARE( ts2.count(), someLanguages().count() );
    QVERIFY( !ts2.isEmpty() );

    // This key doesn't exist
    CalamaresUtils::Locale::TranslatedString ts3( map, "front" );
    QVERIFY( ts3.isEmpty() );
    QCOMPARE( ts3.count(), 1 );  // The empty string
}

void
LocaleTests::testRegions()
{
    using namespace CalamaresUtils::Locale;
    RegionsModel regions;

    QVERIFY( regions.rowCount( QModelIndex() ) > 3 );  // Africa, America, Asia

    QStringList names;
    for ( int i = 0; i < regions.rowCount( QModelIndex() ); ++i )
    {
        QVariant name = regions.data( regions.index( i ), RegionsModel::NameRole );
        QVERIFY( name.isValid() );
        QVERIFY( !name.toString().isEmpty() );
        names.append( name.toString() );
    }

    QVERIFY( names.contains( "America" ) );
    QVERIFY( !names.contains( "UTC" ) );
}


static void
displayedNames( QAbstractItemModel& model, QStringList& names )
{
    names.clear();
    for ( int i = 0; i < model.rowCount( QModelIndex() ); ++i )
    {
        QVariant name = model.data( model.index( i, 0 ), Qt::DisplayRole );
        QVERIFY( name.isValid() );
        QVERIFY( !name.toString().isEmpty() );
        names.append( name.toString() );
    }
}

void
LocaleTests::testSimpleZones()
{
    using namespace CalamaresUtils::Locale;
    ZonesModel zones;

    QVERIFY( zones.rowCount( QModelIndex() ) > 24 );

    QStringList names;
    displayedNames( zones, names );
    QVERIFY( names.contains( "Amsterdam" ) );
    if ( !names.contains( "New York" ) )
    {
        for ( const auto& s : names )
        {
            if ( s.startsWith( 'N' ) )
            {
                cDebug() << s;
            }
        }
    }
    QVERIFY( names.contains( "New York" ) );
    QVERIFY( !names.contains( "America" ) );
    QVERIFY( !names.contains( "New_York" ) );
}

void
LocaleTests::testComplexZones()
{
    using namespace CalamaresUtils::Locale;
    ZonesModel zones;
    RegionalZonesModel europe( &zones );

    QStringList names;
    displayedNames( zones, names );
    QVERIFY( names.contains( "New York" ) );
    QVERIFY( names.contains( "Prague" ) );
    QVERIFY( names.contains( "Abidjan" ) );

    // No region set
    displayedNames( europe, names );
    QVERIFY( names.contains( "New York" ) );
    QVERIFY( names.contains( "Prague" ) );
    QVERIFY( names.contains( "Abidjan" ) );

    // Now filter
    europe.setRegion( "Europe" );
    displayedNames( europe, names );
    QVERIFY( !names.contains( "New York" ) );
    QVERIFY( names.contains( "Prague" ) );
    QVERIFY( !names.contains( "Abidjan" ) );

    europe.setRegion( "America" );
    displayedNames( europe, names );
    QVERIFY( names.contains( "New York" ) );
    QVERIFY( !names.contains( "Prague" ) );
    QVERIFY( !names.contains( "Abidjan" ) );

    europe.setRegion( "Africa" );
    displayedNames( europe, names );
    QVERIFY( !names.contains( "New York" ) );
    QVERIFY( !names.contains( "Prague" ) );
    QVERIFY( names.contains( "Abidjan" ) );
}

void
LocaleTests::testTZLookup()
{
    using namespace CalamaresUtils::Locale;
    ZonesModel zones;

    QVERIFY( zones.find( "America", "New_York" ) );
    QCOMPARE( zones.find( "America", "New_York" )->zone(), QStringLiteral( "New_York" ) );
    QCOMPARE( zones.find( "America", "New_York" )->tr(), QStringLiteral( "New York" ) );

    QVERIFY( !zones.find( "Europe", "New_York" ) );
    QVERIFY( !zones.find( "America", "New York" ) );
}

void
LocaleTests::testTZIterator()
{
    using namespace CalamaresUtils::Locale;
    const ZonesModel zones;

    QVERIFY( zones.find( "Europe", "Rome" ) );

    int count = 0;
    bool seenRome = false;
    bool seenGnome = false;
    for ( auto it = zones.begin(); it; ++it )
    {
        QVERIFY( *it );
        QVERIFY( !( *it )->zone().isEmpty() );
        seenRome |= ( *it )->zone() == QStringLiteral( "Rome" );
        seenGnome |= ( *it )->zone() == QStringLiteral( "Gnome" );
        count++;
    }

    QVERIFY( seenRome );
    QVERIFY( !seenGnome );
    QCOMPARE( count, zones.rowCount( QModelIndex() ) );

    QCOMPARE( zones.data( zones.index( 0 ), ZonesModel::RegionRole ).toString(), QStringLiteral( "Africa" ) );
    QCOMPARE( ( *zones.begin() )->zone(), QStringLiteral( "Abidjan" ) );
}

void
LocaleTests::testLocationLookup_data()
{
    QTest::addColumn< double >( "latitude" );
    QTest::addColumn< double >( "longitude" );
    QTest::addColumn< QString >( "name" );

    QTest::newRow( "London" ) << 50.0 << 0.0 << QString( "London" );
    QTest::newRow( "Tarawa E" ) << 0.0 << 179.0 << QString( "Tarawa" );
    QTest::newRow( "Tarawa W" ) << 0.0 << -179.0 << QString( "Tarawa" );

    QTest::newRow( "Johannesburg" ) << -26.0 << 28.0 << QString( "Johannesburg" );  // South Africa
    QTest::newRow( "Maseru" ) << -29.0 << 27.0 << QString( "Maseru" );  // Lesotho
    QTest::newRow( "Windhoek" ) << -22.0 << 17.0 << QString( "Windhoek" );  // Namibia
    QTest::newRow( "Port Elisabeth" ) << -33.0 << 25.0 << QString( "Johannesburg" );  // South Africa
    QTest::newRow( "Cape Town" ) << -33.0 << 18.0 << QString( "Johannesburg" );  // South Africa
}

void
LocaleTests::testLocationLookup()
{
    const CalamaresUtils::Locale::ZonesModel zones;

    QFETCH( double, latitude );
    QFETCH( double, longitude );
    QFETCH( QString, name );

    const auto* zone = zones.find( latitude, longitude );
    QVERIFY( zone );
    QCOMPARE( zone->zone(), name );
}

void
LocaleTests::testLocationLookup2()
{
    // Official
    // ZA      -2615+02800     Africa/Johannesburg
    // Spot patch
    //     "ZA -3230+02259 Africa/Johannesburg\n";

    const CalamaresUtils::Locale::ZonesModel zones;
    const auto* zone = zones.find( -26.15, 28.00 );
    QCOMPARE( zone->zone(), QString( "Johannesburg" ) );
    // The TZ data sources use minutes-and-seconds notation,
    // so "2615" is 26 degrees, 15 minutes, and 15 minutes is
    // one-quarter of a degree.
    QCOMPARE( zone->latitude(), -26.25 );
    QCOMPARE( zone->longitude(), 28.00 );

    // Elsewhere in South Africa
    const auto* altzone = zones.find( -32.0, 22.0 );
    QCOMPARE( altzone, zone );  // same pointer
    QCOMPARE( altzone->zone(), QString( "Johannesburg" ) );
    QCOMPARE( altzone->latitude(), -26.25 );
    QCOMPARE( altzone->longitude(), 28.00 );

    altzone = zones.find( -29.0, 27.0 );
    QCOMPARE( altzone->zone(), QString( "Maseru" ) );
    // -2928, that's -29 and 28/60 of a degree, is almost half, but we don't want
    //   to fall foul of variations in double-precision
    QCOMPARE( trunc( altzone->latitude() * 1000.0 ), -29466 );
}


QTEST_GUILESS_MAIN( LocaleTests )

#include "utils/moc-warnings.h"

#include "Tests.moc"
