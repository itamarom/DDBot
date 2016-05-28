import os
import sys
import math
import time
import random
import pygame
import socket
import struct
import select
import win32api

SERVER_PORT = 1337

pygame.font.init()
my_font = pygame.font.SysFont("arial", 32)

DigitalRead, DigitalWrite, ServoWrite = xrange(3)

LEFT_MOTOR_LOCATION = (730, 200)
RIGHT_MOTOR_LOCATION = (490, 220)

ARM_SIZE = (10, 100)
SCREEN_SIZE = (1024, 768)

BGCOLOR = (255, 255, 255)

LOOP_MS = 20

BASE_SPEED = 8

MICROSWITCH_ANGLE_RANGE = 15

randfloat = random.uniform

def rotPoint(point, axis, ang):
    """ Orbit. calcs the new loc for a point that rotates a given num of degrees around an axis point,
    +clockwise, -anticlockwise -> tuple x,y
    """
    x, y = point[0] - axis[0], point[1] - axis[1]
    radius = math.sqrt(x*x + y*y) # get the distance between points

    RAng = math.radians(ang)       # convert ang to radians.

    h = axis[0] + ( radius * math.cos(RAng) )
    v = axis[1] + ( radius * math.sin(RAng) )

    return h, v

def calcServoSpeed(num):
    val = (float(num) - 90) / 90
    if val > 1: return 1
    if val < -1: return -1
    return val

class Game(object):
    def __init__(self, size=SCREEN_SIZE, loopDelay=LOOP_MS):
        self.screen = pygame.display.set_mode(size)
        self.running = True
        self.lastLoop = time.time()
        self.loopDelay = loopDelay / 1000.0

    def run(self):
        while self.running:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.running = False
                    continue
                else:
                    self.handleEvent(event)
                     
            currentTime = time.time()
            if currentTime - self.lastLoop < self.loopDelay:
                time.sleep(self.lastLoop + self.loopDelay - currentTime)
            self.lastLoop = currentTime
            self.update()
            self.draw()
            pygame.display.flip()
            
        os._exit(0)
    
    def handleEvent(self, event):
        raise NotImplementedError()

    def draw(self):
        raise NotImplementedError()
  
class SpinningArm(object):
    def __init__(self, position, game, switchAngle=50, angle=0, speedFactor=None, reverse=False):
        self.speedFactor = speedFactor if speedFactor is not None else random.randint(92, 100) / 100.0
        if reverse:
           self.speedFactor *= -1
        self.position = position
        self.game = game
        self.size = ARM_SIZE
        self.angle = angle
        self.currentInput = 0
        self.switchAngle = switchAngle
        self.switchPos = map(int, rotPoint((position[0], position[1] - 50), position, -switchAngle))
        
    def update(self):
        self.angle = (self.angle + BASE_SPEED * randfloat(0.97, 1.03) * self.currentInput * self.speedFactor) % 360
        # self.angle = (self.angle + BASE_SPEED * self.currentInput * self.speedFactor) % 360
        
    def draw(self):
        rot_img = pygame.transform.rotate(self.game.arm_img, self.angle)
        rect = rot_img.get_rect()
        rect.center = self.position
        self.game.screen.blit(rot_img, rect)
        pygame.draw.circle(self.game.screen, (0, 0, 255), self.switchPos, 5, 0)
        
    def setServo(self, value):
        self.currentInput = calcServoSpeed(value)
        
    def getMicroswitchValue(self):
        return int(abs(self.angle - self.switchAngle) < MICROSWITCH_ANGLE_RANGE)
        
class KeyState(object):
    def __init__(self, vkey):
        self.vkey = vkey
        
    def get(self):
        return win32api.GetAsyncKeyState(vkey)
  
class DDBot(Game):
    def __init__(self, no_network=False):
        super(DDBot, self).__init__()
        
        self.bg_img = pygame.image.load("bgcut.png").convert()
        self.arm_img = pygame.image.load("arm2.png").convert_alpha()
        
        self.right = SpinningArm(RIGHT_MOTOR_LOCATION, self, reverse=True, switchAngle=160)
        self.left = SpinningArm(LEFT_MOTOR_LOCATION, self, switchAngle=190)
        self.breadBoard = { 10: self.right.setServo, 11: self.left.setServo,
                            12: self.left.getMicroswitchValue, 13: self.right.getMicroswitchValue}

        self.listening_socket = socket.socket()
        self.listening_socket.bind(('', SERVER_PORT))
        self.listening_socket.listen(5)
        self.client = None
        
        self.bg_rect = self.bg_img.get_rect()
        #self.bg_rect.left = -720
        self.bg_rect.top = 80
        
        self.no_network = no_network
        
        if not no_network:
            self.acceptClient()
        
    def acceptClient(self):
        print "Accepting...",
        self.client, addr = self.listening_socket.accept()
        print "Connected!"
        
    def readFromClient(self):
        rl, _, _ = select.select([self.client], [], [], 0)
        if not rl: return
        try:
            data = self.client.recv(4*3)
        except socket.error as e:
            if e.errno != 10053:
                raise e
            else:
                print "Client disconnected.",
                self.acceptClient()
                return
        if not data:
            print "Client disconnected.",
            self.acceptClient()
            return
        cmdType, pinNumber, value = struct.unpack("<III", data)
        result = self.handleCommand(cmdType, pinNumber, value)
        if result is not None:
            self.client.sendall(struct.pack("<I", result))
        
    def handleCommand(self, cmdType, pinNumber, value):
        #print "* Got command", cmdType, pinNumber, value
        
        if cmdType == ServoWrite:
            print "ServoWrite(%d, %d)" % (pinNumber, value)
            self.breadBoard[pinNumber](value)
        elif cmdType == DigitalRead:
            print "DigitalRead(%d) = " % (pinNumber, ),
            value = self.breadBoard[pinNumber]()
            print value
            return value
        elif cmdType == DigitalWrite:
            #print "DigitalWrite(%d, %d), ignoring" % (pinNumber, value) 
            pass
        
    def handleEvent(self, event):
        return
        #if event.type == pygame.MOUSEMOTION:
        #    print event.pos[0], event.pos[1]
        #elif event.type == pygame.MOUSEBUTTONDOWN:
        #    print event.pos[0], event.pos[1]
                
    def update(self):
        if not self.no_network:
            self.readFromClient()
        self.left.update()
        self.right.update()
        #print "Update", time.time()
        
    def draw(self):
        self.screen.fill(BGCOLOR)

        self.screen.blit(self.bg_img, self.bg_rect)
        self.left.draw()
        self.right.draw()
        
        #screen.blit(bg_img, (0, 0))
        #
        #for hit in bullet_hits:
        #    print hit
        #    screen.blit(hit_img, hit)
        #
        #screen.blit(target_img, target_rect)
        #screen.blit(aim_img, aim_rect)
        #
        #screen.blit(my_font.render("Score: " + str(score),
        #                           1, (255, 0, 0)), (50, 50))
        
def main():
    game = DDBot(no_network='-n' in sys.argv)
    game.run()

if __name__ == "__main__":
    main()