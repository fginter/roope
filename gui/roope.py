from PyQt4.QtGui import *
from PyQt4.QtCore import Qt, pyqtSlot
from ui_roope import *
import sys
import struct
import serial.tools.list_ports
import serial
import math
import time

DOWN=180
UP=0

#COMMAND:
# DSSAAPB;   (D)rive (S)teps (A)ngle 2bytes, 0-360   (P)en 1byte 0-255   (B)ackwards 1byte 0/1

class Roope(QMainWindow):
    
    def __init__(self):
        QMainWindow.__init__(self)
        self.gui=Ui_MainWindow()
        self.gui.setupUi(self)
        self.scene=QGraphicsScene()
        self.gui.g_view.setScene(self.scene)
        self.port=None
        self.connect_to_port("/dev/ttyUSB1")

    # def refreshSerialPorts(self):
    #     self.gui.portList.clear()
    #     for path,comment,HWID in serial.tools.list_ports.comports():
    #         if not "USB" in path:
    #             continue
    #         self.gui.portList.addItem(path)
    
    def connect_to_port(self,port):
        port=str(port)
        if self.port!=None:
            self.port.close()
            self.port=None
        if port:
            self.port=serial.Serial(port,9600,bytesize=serial.EIGHTBITS,parity=serial.PARITY_NONE,stopbits=serial.STOPBITS_ONE)
            time.sleep(3)
            print >> sys.stderr, "Connected to", port

    def load(self,fig_file,max_x=2000,max_y=300):
        img=QImage(fig_file)
        img2=img.scaled(max_x,max_y,Qt.KeepAspectRatio)
        self.image=img2
        #img2=self.BW(img2)
        pix_map=QPixmap(img2)
        self.scene.addPixmap(pix_map)
        self.gui.g_view.fitInView(self.scene.itemsBoundingRect(),Qt.KeepAspectRatio)
        
    def BW(self,img):
        img2=img.scaled(img.width()*5,img.height()*5)
        for x2 in xrange(img2.width()):
            for y2 in xrange(img2.height()):
                img2.setPixel(x2,y2,qRgb(255,255,255))
        for x in xrange(img.width()):
            for y in xrange(img.height()):
                p=img.pixel(x,y)
                gray=5-(qGray(p)//51)
                assert gray<=6
                for x2 in xrange(x*5,x*5+gray):
                    for y2 in xrange(y*5,y*5+5):
                        img2.setPixel(x2,y2,qRgb(0,0,0))
        return img2

    def comm(self,s):
        print >> sys.stderr, "Sending ",repr(s)
        print >> sys.stderr, "Written", self.port.write(s)
        #Now wait for "OK"
        while True:
            b=self.port.read()
            if b!=";":
                sys.stderr.write(b)
                sys.stderr.flush()
            else:
                break
        print >> sys.stderr
        print >> sys.stderr, "Comm done"
        sys.stderr.flush()

    def move_pixel(self,steps,angle,pen,backwards):
        print "S=", steps, "A=", angle, "P=", pen, "B=", backwards
        command=struct.pack("<cHHBBc","D",steps,angle,pen,backwards,";") #c character, H unsigned 2B int, B unsigned byte  "<" little endian
        self.comm(command)

    def follow_line(self,x=0,from_y=0,direction=DOWN,steps=50):
        if direction==DOWN:
            ys=xrange(from_y,self.image.height())
            backwards=1
        elif direction==UP:
            ys=xrange(from_y,-1,-1)
            backwards=0
        for y in ys:
            self.move_pixel(steps,0,qGray(self.image.pixel(x,y)),backwards)
        self.side_step(direction)

    def side_step(self,direction=DOWN,steps=10,angle=20):
        angleRAD=math.radians(90-angle)
        traverse=int(steps/math.cos(angleRAD)) #How many steps to travel under angle?
        back=int(steps*math.tan(angleRAD))
        if direction==DOWN:
            self.move_pixel(traverse,360-angle,0,True)
            self.move_pixel(back,0,0,False)
        elif direction==UP:
            self.move_pixel(traverse,angle,0,False)
            self.move_pixel(back,0,0,True)

def main(app):
    roope=Roope()
    roope.show()
    #roope.load("20140617_010845.jpg")
    roope.move_pixel(100,0,1,0)
    roope.side_step(UP,100)
    roope.move_pixel(100,0,1,1)
    for i in range(10):
        roope.move_pixel(2000,0,i%2,0)
    #roope.follow_line()
    #roope.load("photo.jpg")
    #return app.exec_()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    sys.exit(main(app))
