# Makefile para Windows

PROG = robot-football.exe

FONTES = main.c opengl.c 

FONTES_SOIL = SOIL/SOIL.c SOIL/image_DXT.c SOIL/image_helper.c SOIL/stb_image_aug.c

FONTES_CM = chipmunk/chipmunk.c chipmunk/cpArbiter.c chipmunk/cpArray.c chipmunk/cpBBTree.c \
chipmunk/cpBody.c chipmunk/cpCollision.c chipmunk/cpConstraint.c \
chipmunk/cpDampedRotarySpring.c chipmunk/cpDampedSpring.c chipmunk/cpGearJoint.c \
chipmunk/cpGrooveJoint.c chipmunk/cpHashSet.c chipmunk/cpHastySpace.c chipmunk/cpMarch.c \
chipmunk/cpPinJoint.c chipmunk/cpPivotJoint.c chipmunk/cpPolyShape.c chipmunk/cpPolyline.c \
chipmunk/cpRatchetJoint.c chipmunk/cpRobust.c chipmunk/cpRotaryLimitJoint.c \
chipmunk/cpShape.c chipmunk/cpSimpleMotor.c chipmunk/cpSlideJoint.c chipmunk/cpSpace.c \
chipmunk/cpSpaceComponent.c chipmunk/cpSpaceDebug.c chipmunk/cpSpaceHash.c \
chipmunk/cpSpaceQuery.c chipmunk/cpSpaceStep.c chipmunk/cpSpatialIndex.c chipmunk/cpSweep1D.c

OBJETOS = $(FONTES:.c=.o)
OBJETOS_SOIL = $(FONTES_SOIL:.c=.o)
OBJETOS_CM = $(FONTES_CM:.c=.o)

CFLAGS = -O3 -g -Iinclude -Iinclude/chipmunk -Iinclude/SOIL # -Wall -g  # Todas as warnings, infos de debug

LDFLAGS = -Llib -lfreeglut -lopengl32 -lglu32 -lm

CC = gcc

$(PROG): $(OBJETOS) $(OBJETOS_SOIL) $(OBJETOS_CM)
	gcc $(CFLAGS) $(OBJETOS) $(OBJETOS_SOIL) $(OBJETOS_CM) -o $@ $(LDFLAGS)

clean:
	-@ del $(OBJETOS) chipmunk\*.o SOIL\*.o $(PROG)
