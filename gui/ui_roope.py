# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui_roope.ui'
#
# Created: Tue Aug 26 00:13:19 2014
#      by: PyQt4 UI code generator 4.10.4
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName(_fromUtf8("MainWindow"))
        MainWindow.resize(689, 625)
        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName(_fromUtf8("centralwidget"))
        self.verticalLayout = QtGui.QVBoxLayout(self.centralwidget)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.position_label = QtGui.QLabel(self.centralwidget)
        self.position_label.setObjectName(_fromUtf8("position_label"))
        self.verticalLayout.addWidget(self.position_label)
        self.g_view = QtGui.QGraphicsView(self.centralwidget)
        self.g_view.setObjectName(_fromUtf8("g_view"))
        self.verticalLayout.addWidget(self.g_view)
        self.frame = QtGui.QFrame(self.centralwidget)
        self.frame.setFrameShape(QtGui.QFrame.StyledPanel)
        self.frame.setFrameShadow(QtGui.QFrame.Raised)
        self.frame.setObjectName(_fromUtf8("frame"))
        self.gridLayout = QtGui.QGridLayout(self.frame)
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.verticalCorrection = QtGui.QDoubleSpinBox(self.frame)
        self.verticalCorrection.setMaximum(200.0)
        self.verticalCorrection.setSingleStep(0.1)
        self.verticalCorrection.setProperty("value", 100.0)
        self.verticalCorrection.setObjectName(_fromUtf8("verticalCorrection"))
        self.gridLayout.addWidget(self.verticalCorrection, 0, 1, 1, 1)
        self.label = QtGui.QLabel(self.frame)
        self.label.setObjectName(_fromUtf8("label"))
        self.gridLayout.addWidget(self.label, 0, 0, 1, 1)
        self.sideStepCorrection = QtGui.QDoubleSpinBox(self.frame)
        self.sideStepCorrection.setMaximum(200.0)
        self.sideStepCorrection.setSingleStep(0.1)
        self.sideStepCorrection.setProperty("value", 100.0)
        self.sideStepCorrection.setObjectName(_fromUtf8("sideStepCorrection"))
        self.gridLayout.addWidget(self.sideStepCorrection, 1, 1, 1, 1)
        self.label_2 = QtGui.QLabel(self.frame)
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.gridLayout.addWidget(self.label_2, 1, 0, 1, 1)
        self.verticalLayout.addWidget(self.frame)
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtGui.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 689, 27))
        self.menubar.setObjectName(_fromUtf8("menubar"))
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtGui.QStatusBar(MainWindow)
        self.statusbar.setObjectName(_fromUtf8("statusbar"))
        MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow", None))
        self.position_label.setText(_translate("MainWindow", "TextLabel", None))
        self.label.setText(_translate("MainWindow", "Vertical correction [%]", None))
        self.label_2.setText(_translate("MainWindow", "Sidestep correction [%]", None))

