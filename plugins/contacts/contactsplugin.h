/**
 * Copyright 2018 Simon Redman <simon@ergotech.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CONTACTSPLUGIN_H
#define CONTACTSPLUGIN_H

class QObject;
#include <QStandardPaths>

#include <core/kdeconnectplugin.h>

/**
 * Used to request the device send the unique ID and last-changed timestamp of every contact
 */
#define PACKET_TYPE_CONTACTS_REQUEST_ALL_UIDS_TIMESTAMP QStringLiteral("kdeconnect.contacts.request_all_uids_timestamps")

/**
 * Used to request the vcards for the contacts corresponding to a list of UIDs
 *
 * It shall contain the key "uids", which will have a list of uIDs (long int, as string)
 */
#define PACKET_TYPE_CONTACTS_REQUEST_VCARDS_BY_UIDS QStringLiteral("kdeconnect.contacts.request_vcards_by_uid")

/**
 * Response indicating the package contains a list of all contact uIDs and last-changed timestamps
 *
 * It shall contain the key "uids", which will mark a list of uIDs (long int, as string)
 * then, for each UID, there shall be a field with the key of that UID and the value of the timestamp (int, as string)
 *
 * For example:
 * ( 'uids' : ['1', '3', '15'],
 *  '1'  : '973486597',
 *  '3'  : '973485443',
 *  '15' : '973492390' )
 *
 * The returned IDs can be used in future requests for more information about the contact
 */
#define PACKAGE_TYPE_CONTACTS_RESPONSE_UIDS_TIMESTAMPS QStringLiteral("kdeconnect.contacts.response_uids_timestamps")

/**
 * Response indicating the package contains a list of contact vcards
 *
 * It shall contain the key "uids", which will mark a list of uIDs (long int, as string)
 * then, for each UID, there shall be a field with the key of that UID and the value of the remote's vcard for that contact
 *
 * For example:
 * ( 'uids' : ['1', '3', '15'],
 *  '1'  : 'BEGIN:VCARD\n....\nEND:VCARD',
 *  '3'  : 'BEGIN:VCARD\n....\nEND:VCARD',
 *  '15' : 'BEGIN:VCARD\n....\nEND:VCARD' )
 */
#define PACKET_TYPE_CONTACTS_RESPONSE_VCARDS QStringLiteral("kdeconnect.contacts.response_vcards")

/**
 * Where the synchronizer will write vcards and other metadata
 * TODO: Per-device folders since each device *will* have different uIDs
 */
Q_GLOBAL_STATIC_WITH_ARGS(
        QString, vcardsLocation,
        (QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + ("/kpeoplevcard")))

#define VCARD_EXTENSION QStringLiteral(".vcf")
#define METADATA_EXTENSION QStringLiteral(".meta")

typedef QString uID;
Q_DECLARE_METATYPE(uID)

typedef QStringList uIDList_t;
Q_DECLARE_METATYPE(uIDList_t)

class Q_DECL_EXPORT ContactsPlugin
    : public KdeConnectPlugin
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kdeconnect.device.contacts")

public:
    explicit ContactsPlugin(QObject* parent, const QVariantList& args);

    bool receivePacket(const NetworkPacket& np) override;
    void connected() override;

    QString dbusPath () const override;

protected:
    /**
     * Path where this instance of the plugin stores its synchronized contacts
     */
    QString vcardsPath;

public Q_SLOTS:

    /**
     * 	Query the remote device for all its uIDs and last-changed timestamps, then:
     *      Delete any contacts which are known locally but not reported by the remote
     *      Update any contacts which are known locally but have an older timestamp
     *      Add any contacts which are not known locally but are reported by the remote
     */
    Q_SCRIPTABLE void synchronizeRemoteWithLocal();

public:
Q_SIGNALS:
    /**
     * Emitted to indicate that we have locally cached all remote contacts
     *
     * @param newContacts The list of just-synchronized contacts
     */
    Q_SCRIPTABLE void localCacheSynchronized(const uIDList_t& newContacts);

protected:

    /**
     *	Handle a packet of type PACKAGE_TYPE_CONTACTS_RESPONSE_UIDS_TIMESTAMPS
     *
     *  For every uID in the reply:
     *      Delete any from local storage if it does not appear in the reply
     *      Compare the modified timestamp for each in the reply and update any which should have changed
     *      Request the details any IDs which were not locally cached
     */
    bool handleResponseUIDsTimestamps(const NetworkPacket&);

    /**
     *  Handle a packet of type PACKET_TYPE_CONTACTS_RESPONSE_VCARDS
     */
    bool handleResponseVCards(const NetworkPacket&);

    /**
     * Send a request-type packet which contains no body
     *
     * @return True if the send was successful, false otherwise
     */
    bool sendRequest(const QString& packetType);

    /**
     * Send a request-type packet which has a body with the key 'uids' and the value the list of
     * specified uIDs
     *
     * @param packageType Type of package to send
     * @param uIDs List of uIDs to request
     * @return True if the send was successful, false otherwise
     */
    bool sendRequestWithIDs(const QString& packetType, const uIDList_t& uIDs);
};

#endif // CONTACTSPLUGIN_H
