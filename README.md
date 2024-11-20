# xdg-desktop-portal-filechooser

A customizable file dialog chooser for use with xdg-desktop-portal.<br>
Default is to use the built in custom filed dialog, you can also select a qt filedialog, with more to come.<br>
<br>
<b>Installation:</b><br>
<code>
make<br>
sudo make install<br>
</code><br>
To select what file dialog to use create xdgfilechooser.conf in ~/.config like so:<br>
<code>#!/bin/bash<br>
<br>
#USEDIALOG=qtdialog<br>
USEDIALOG=customdialog</code><br>
<br>
Comment/uncomment your prefered filedialog<br>
<br>
To start the portal run:<br>
<code>SFM_FILE_SCRIPT='/usr/share/filechooserportal/xdgportalscript' '/usr/libexec/filechooser'</code><br>
<br>
Some where in your start up scripts ( this will vary by distro, probably adding to rc.local will work fine ).<br>
<br>
<br>
<b>TODO:</b><br>
gtk chooser.<br>
lxqt chooser.<br>
