TEMPLATE = subdirs

SUBDIRS += base


# TODO use PLUGINS variable instead of polluting CONFIG
mosek {
    SUBDIRS += mosek
}

kinect {
    SUBDIRS += kinect
}

opencv {
    SUBDIRS += opencv 
}


