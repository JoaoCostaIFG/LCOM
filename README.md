# LCOM  

The report explains the LCOM project in detail (in case you're interested).  
To compile and run the game:  
- cd to the **src/** directory  
- write **make clean && make**  
- **lcom_run proj "<path_to_the_resources_directory> <video_mode>"**.
It should be noted that these command line arguments are optional.  

# Grades

- Lab. 2 - 99/100;
- Lab. 3 - 16/100 (we submited the wrong version, which didn't even compile);
- Lab. 4 - 87/100;
- Lab. 5 - 92/100;
- Final Project - 19.77/20.

The code for the labs was improved a lot for the final project, so you
should check the code on the project instead of the labs.

# Final project details

The game is highly configurable and has a both a **singleplayer** and a **multiplayer**
modes.

In the **singleplayer** mode, you play as a snake and must face increasingly difficult
waves of enemies, that will try to kill you, by shooting them. The more you shoot,
the smaller you become. When enemies die, they drop strawberries that you can eat
to increase your size. You die when you get damaged if only your head remains
(minimum size).

In the **multiplayer** mode, you play against a friend in a different computer
(connect through a serial port). The game mechanics remain mostly the same, with
the exception that enemy scaling has been removed. To win, you must kill the other
player (or wait for him to die to his enemies), by shooting/colliding with him.

# MINIX image  

The MINIX image used as been added to the repository for preservation
(if something fails in the future, run the image without internet connection).

# Contributors

This project was developed with my good friend [João Lucas](https://github.com/joaolucasmartins)
for the class (work in pairs).  
I also promissed my friend [Ana](https://github.com/anaines14) that I'd
give her a place in the credits (as a joke) for an idea we discussed
so here it is.
