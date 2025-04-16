/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the FOO module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

function Component()
{
    // constructor
    component.loaded.connect(this, Component.prototype.loaded);
    if (!installer.addWizardPage(component, "Page", QInstaller.TargetDirectory))
        console.log("Could not add the dynamic page.");
}

Component.prototype.isDefault = function()
{
    // select the component by default
    return true;
}

Component.prototype.createOperations = function()
{
    try {
        // call the base create operations function
        component.createOperations();
    } catch (e) {
        console.log(e);
    }

    if (systemInfo.productType === "windows") {
    // pop up create shortcut question
    var result = QMessageBox.question(
        "shortcut.question",                 // internal name
        "Create Desktop Shortcut",           // dialog title
        "Do you want to create a desktop shortcut?", // question
        QMessageBox.Yes | QMessageBox.No);

        if (result == QMessageBox.Yes) {
            // create the shortcut
            component.addOperation("CreateShortcut",
                "@TargetDir@/Replica-Reaper.exe",
                "@DesktopDir@/Replica Reaper.lnk",
                "iconPath=@TargetDir@/rr-logo.ico" );
        }
    }
}

Component.prototype.loaded = function ()
{
    var page = gui.pageByObjectName("DynamicPage");
    if (page != null) {
        console.log("Connecting the dynamic page entered signal.");
        page.entered.connect(Component.prototype.dynamicPageEntered);
    }
}

Component.prototype.dynamicPageEntered = function ()
{
    var pageWidget = gui.pageWidgetByObjectName("DynamicPage");
    if (pageWidget != null) {
        console.log("Setting the widgets label text.")
        pageWidget.m_pageLabel.text =  
             "<p style='text-align:center; font-size:14px;'>Welcome to Replica Reaper, a file duplicate detection manager <br> capable of running in the background</p>"
        + "<p style='text-align:justify; margin-top:10px; font-size:12px;'>"
        + "This software was developed by <b>Chris Lambert</b>, <b>Braden Maillet</b>, <b>Isaiah Andrade</b>, <b>Gavin Ippolito</b>,<br> and <b>Evan Cassanta</b>,<br>"
        + "students at <i><a href='https://www.uml.edu' style='color:#2e7bd1; text-decoration:none;'>University of Massachusetts, Lowell</a></i>,<br>"
        + "as part of the <i>Software Engineering II Project Sequence</i>."
        + "</p>"
        + "<p style='text-align:center; margin-top:16px;'>"
        + "<a href='https://github.com/chrislambert3/Replica-Reaper' "
        + "   style='color:#2e7bd1; font-weight:bold; text-decoration:none;'>"
        + "View more Information on Github"
        + "</a>"
        + "</p>"
        + "<p style='text-align:left; margin-top:12px;'>An uninstaller <i>rr-maintenancetool</i> will be installed in the application directory</p>";

        pageWidget.m_pageLabel.openExternalLinks = true;  // enable clickable link
  
    }
}

