from PyQt4.QtGui import *
from PyQt4.QtCore import Qt
from ui_roope import *
import sys
import struct

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

    # def comm(self,s):
    #     self.port.write(s)
    #     self.port.flush()
    #     #Now wait for "OK"
    #     b=self.port.read()
    #     assert b==";"

    def comm(self,s):
        print repr(s)

    def move_pixel(self,steps,angle,pen,backwards):
        command=struct.pack("<cHHBBc","D",steps,angle,pen,backwards,";")
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
            
        

def main(app):
    roope=Roope()
    roope.show()
    roope.load("20140617_010845.jpg")
    roope.follow_line()
    #roope.load("photo.jpg")
    return app.exec_()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    sys.exit(main(app))
