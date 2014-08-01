from PyQt4.QtGui import *
from PyQt4.QtCore import Qt, pyqtSlot, QThread
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

class DrawThread(QThread):

    def __init__(self,roope):
        QThread.__init__(self)
        self.roope=roope

    def run(self):
        self.roope.draw_fig()
        self.terminate()


class Roope(QMainWindow):
    
    def __init__(self,pixel_v_steps,pixel_h_steps):
        QMainWindow.__init__(self)
        self.gui=Ui_MainWindow()
        self.gui.setupUi(self)
        self.scene=QGraphicsScene()
        self.gui.g_view.setScene(self.scene)
        self.port=None
        self.connect_to_port()
        self.pixel_v_steps=pixel_v_steps
        self.pixel_h_steps=pixel_h_steps
        self.vertical_correction=100.0
        self.draw_t=DrawThread(self)
        

    # def refreshSerialPorts(self):
    #     self.gui.portList.clear()
    #     for path,comment,HWID in serial.tools.list_ports.comports():
    #         if not "USB" in path:
    #             continue
    #         self.gui.portList.addItem(path)

    def connect_to_port(self,port=None):
        if self.port!=None:
            self.port.close()
            self.port=None
        if port==None: #Try to auto-detect
            for path,comment,HWID in serial.tools.list_ports.comports():
                if "ttyUSB" in path:
                    port=path
                    break
        if port:
            self.port=serial.Serial(port,9600,bytesize=serial.EIGHTBITS,parity=serial.PARITY_NONE,stopbits=serial.STOPBITS_ONE)
            time.sleep(3)
            print >> sys.stderr, "Connected to", port
        else:
            print >> sys.stderr, "Couldn't find any port to connect to"

    def load(self,fig_file,height=300):
        img=QImage(fig_file)
        img2=img.scaledToHeight(300,Qt.SmoothTransformation)
        self.image=img2
        #img2=self.BW(img2)
        pix_map=QPixmap(img2)
        self.scene.addPixmap(pix_map)
        #self.gui.g_view.fitInView(self.scene.itemsBoundingRect(),Qt.KeepAspectRatio)
        
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
        self.port.write(s)
        #Now wait for "OK"
        while True:
            b=self.port.read()
            if b!=";":
                #sys.stderr.write(b)
                sys.stderr.flush()
            else:
                break

    def move_pixel(self,steps,angle,pen,backwards):
        print "S=", steps, "A=", angle, "P=", pen, "B=", backwards
        command=struct.pack("<cHHBBc","D",steps,angle,pen,backwards,";") #c character, H unsigned 2B int, B unsigned byte  "<" little endian
        self.comm(command)

    def draw_fig(self,from_x=0,from_y=0,direction=DOWN):
        xs=range(from_x,self.image.width(),self.pixel_h_steps//self.pixel_v_steps)
        for x in xs:
            print "X=",x, "Image width:", self.image.width(), "Image height:", self.image.height()
            if x==from_x:
                y=from_y
            else:
                if direction==DOWN:
                    y=0
                else:
                    y=self.image.height()-1
            self.follow_line(x,y,direction)
            self.side_step(UP,steps=self.pixel_h_steps,angle=20)
            if direction==DOWN:
                direction=UP
            else:
                direction=DOWN

    def follow_line(self,x=0,from_y=0,direction=DOWN):
        if direction==DOWN:
            ys=xrange(from_y,self.image.height())
            backwards=1
        elif direction==UP:
            ys=xrange(from_y,-1,-1)
            backwards=0
        for y in ys:
            if direction==DOWN:
                step=int(self.pixel_v_steps*self.gui.verticalCorrection.value()/100.0)
            elif direction==UP:
                step=int(self.pixel_v_steps)
            color2=self.image.pixel(x,y)
            self.move_pixel(step,0,255-qGray(color2),backwards)

    def side_step(self,direction,steps,angle):
        angleRAD=math.radians(90-angle)
        traverse=int(steps/math.cos(angleRAD)) #How many steps to travel under angle?
        back=int(steps*math.tan(angleRAD))
        if direction==DOWN:
            self.move_pixel(traverse,360-angle,0,True)
            self.move_pixel(back,0,0,False)
        elif direction==UP:
            self.move_pixel(traverse,angle,0,False)
            self.move_pixel(back*self.gui.verticalCorrection.value()/100.0,0,0,True)

def main(app):
    global draw_t
    roope=Roope(pixel_v_steps=400,pixel_h_steps=400)
    #roope.load("20140617_010845.jpg")
    roope.load("spiral.png")
    roope.show()
    #roope.draw_fig()
    roope.draw_t.start()
    #roope.move_pixel(100,0,1,0)
    #roope.side_step(UP,100)
    #roope.move_pixel(100,0,1,1)
    #for i in range(10):
    #    roope.move_pixel(2000,0,i%2,0)
    #roope.follow_line()
    #roope.load("photo.jpg")
    return app.exec_()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    sys.exit(main(app))
