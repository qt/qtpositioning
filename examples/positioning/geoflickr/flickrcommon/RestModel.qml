// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQml.XmlListModel

//! [restmodel]
XmlListModel {
    property var coordinate

    source: "https://api.flickr.com/services/rest/?" +
            "min_taken_date=2000-01-01+0:00:00&" +
            "extras=date_taken&" +
            "method=flickr.photos.search&" +
            "per_page=30&" +
            "sort=date-taken-desc&" +
            "api_key=e36784df8a03fea04c22ed93318b291c&" +
            "lat=" + coordinate.latitude + "&lon=" + coordinate.longitude;
    query: "/rsp/photos/photo"

    XmlListModelRole { name: "title"; elementName: ""; attributeName: "title" }
    XmlListModelRole { name: "datetaken"; elementName: ""; attributeName: "datetaken" }
    XmlListModelRole { name: "farm"; elementName: ""; attributeName: "farm" }
    XmlListModelRole { name: "server"; elementName: ""; attributeName: "server" }
    XmlListModelRole { name: "id"; elementName: ""; attributeName: "id" }
    XmlListModelRole { name: "secret"; elementName: ""; attributeName: "secret" }
}
//! [restmodel]
