/* === This file is part of Calamares - <https://github.com/calamares> ===
 *
 *   SPDX-FileCopyrightText: 2014-2015 Teo Mrnjavac <teo@kde.org>
 *   SPDX-FileCopyrightText: 2019 Gabriel Craciunescu <crazy@frugalware.org>
 *   SPDX-FileCopyrightText: 2019 Dominic Hayes <ferenosdev@outlook.com>
 *   SPDX-FileCopyrightText: 2017-2018 Adriaan de Groot <groot@kde.org>
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

#ifndef SETTINGS_H
#define SETTINGS_H

#include "DllMacro.h"
#include "modulesystem/Actions.h"
#include "modulesystem/InstanceKey.h"

#include <QObject>
#include <QStringList>


namespace Calamares
{

/** @brief Description of an instance as named in `settings.conf`
 *
 * An instance is an intended-step-in-sequence; it is not yet
 * a loaded module. The instances have config-files and weights
 * which are used by the module manager when loading modules
 * and creating jobs.
 */
class DLLEXPORT InstanceDescription
{
    using InstanceKey = Calamares::ModuleSystem::InstanceKey;

public:
    /** @brief An invalid InstanceDescription
     *
     * Use `fromSettings()` to populate an InstanceDescription and
     * check its validity.
     */
    InstanceDescription() = default;

    /** @brief An InstanceDescription with no special settings.
     *
     * Regardless of @p key being custom, sets weight to 1 and
     * the configuration file to @c key.module() (plus the ".conf"
     * extension).
     *
     * To InstanceDescription is custom if the key is.
     */
    InstanceDescription( const InstanceKey& key );

    static InstanceDescription fromSettings( const QVariantMap& );

    bool isValid() const { return m_instanceKey.isValid(); }

    const InstanceKey& key() const { return m_instanceKey; }
    QString configFileName() const { return m_configFileName; }
    bool isCustom() const { return m_instanceKey.isCustom(); }
    int weight() const { return m_weight < 0 ? 1 : m_weight; }
    bool explicitWeight() const { return m_weight > 0; }

private:
    InstanceKey m_instanceKey;
    QString m_configFileName;
    int m_weight = 0;
};

class DLLEXPORT Settings : public QObject
{
    Q_OBJECT
#ifdef BUILD_AS_TEST
public:
#endif
    explicit Settings( bool debugMode );
    explicit Settings( const QString& settingsFilePath, bool debugMode );

    void setConfiguration( const QByteArray& configData, const QString& explainName );
    void reconcileInstancesAndSequence();

public:
    static Settings* instance();
    /// @brief Find a settings.conf, following @p debugMode
    static Settings* init( bool debugMode );
    /// @brief Explicif filename, debug is always true (for testing)
    static Settings* init( const QString& filename );

    QStringList modulesSearchPaths() const;

    using InstanceDescriptionList = QList< InstanceDescription >;
    /** @brief All the module instances used
     *
     * Each module-instance mentioned in `settings.conf` has an entry
     * in the moduleInstances list -- both custom entries that are
     * in the *instances* section, and each module mentioned in the
     * *sequence*.
     */
    InstanceDescriptionList moduleInstances() const;

    using ModuleSequence = QList< QPair< ModuleSystem::Action, Calamares::ModuleSystem::InstanceKeyList > >;
    /** @brief Representation of *sequence* of execution
     *
     * Each "section" of the *sequence* key in `settings.conf` gets an
     * entry here, stating what kind of action is taken and which modules
     * take part (in order). Each entry in the list is an instance key
     * which can be found in the moduleInstances() list.
     */
    ModuleSequence modulesSequence() const;

    QString brandingComponentName() const;

    /** @brief Are the settings consistent and valid?
     *
     * Checks that at least branding is set, and that the instances
     * and sequence are valid.
     */
    bool isValid() const;

    /** @brief Is this a debugging run?
     *
     * Returns true if Calamares is in debug mode. In debug mode,
     * modules and settings are loaded from more locations, to help
     * development and debugging.
     */
    bool debugMode() const { return m_debug; }

    /** @brief Distinguish "install" from "OEM" modes.
     *
     * Returns true in "install" mode, which is where actions happen
     * in a chroot -- the target system, which exists separately from
     * the source system. In "OEM" mode, returns false and most actions
     * apply to the *current* (host) system.
     */
    bool doChroot() const { return m_doChroot; }

    /** @brief Global setting of prompt-before-install.
     *
     * Returns true when the configuration is such that the user
     * should be prompted one-last-time before any action is taken
     * that really affects the machine.
     */
    bool showPromptBeforeExecution() const { return m_promptInstall; }

    /** @brief Distinguish between "install" and "setup" modes.
     *
     * This influences user-visible strings, for instance using the
     * word "setup" instead of "install" where relevant.
     */
    bool isSetupMode() const { return m_isSetupMode; }

    /** @brief Global setting of disable-cancel: can't cancel ever. */
    bool disableCancel() const { return m_disableCancel; }
    /** @brief Temporary setting of disable-cancel: can't cancel during exec. */
    bool disableCancelDuringExec() const { return m_disableCancelDuringExec; }

    /** @brief Is quit-at-end set? (Quit automatically when done) */
    bool quitAtEnd() const { return m_quitAtEnd; }

private:
    static Settings* s_instance;

    QStringList m_modulesSearchPaths;

    InstanceDescriptionList m_moduleInstances;
    ModuleSequence m_modulesSequence;

    QString m_brandingComponentName;

    bool m_debug;
    bool m_doChroot;
    bool m_isSetupMode;
    bool m_promptInstall;
    bool m_disableCancel;
    bool m_disableCancelDuringExec;
    bool m_quitAtEnd;
};

}  // namespace Calamares

#endif  // SETTINGS_H
