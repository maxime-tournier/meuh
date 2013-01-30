

package.depends = fresh first
package.commands = cd ..; tar cvzf /tmp/meuh.tar.gz meuh/meuh.pro meuh/bin meuh/lib meuh/share

QMAKE_EXTRA_TARGETS += package

