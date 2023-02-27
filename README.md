# Physical Game Demo

This is a test game project written in C++ on a basic physics system built in combination with advanced mathematics and physics. The gameplay is very simple, and the code is extensible, allowing for future iterations.

The objective of the game is simple: we are a little duck. The objective is to go and capture the yellow football ball and bring it to the blue target owner. We can also throw the ball to the owner from a distance, the further away we are the more points we score. Watch out for the patrolling goat enemies!!!



## Hot keys

#### Game operation

[G] - Game over

[P] - Pause

[F1] - Restart game

#### Player Operation

[AWSD] - Move

[Shift] - Accelerate

##### When you touch the ball

[Left] click - Catching the ball

	##### When you hold the ball

[Q] - Reduced force

[E] - Increase force

[Right] click - Kick the ball

[Move mouse] - Adjusting the angle of the kick

## Feature

#### Gravity && Elasticity && Collision Detection && Collision Response

![](D:\NCL\CSC8503\Physical-Systems\Assets\Pictures\gravity.gif)

#### Friction

![](D:\NCL\CSC8503\Physical-Systems\Assets\Pictures\friction.gif)

#### Constraint

![](D:\NCL\CSC8503\Physical-Systems\Assets\Pictures\constrain.gif)

#### Spatial Acceleration

![](D:\NCL\CSC8503\Physical-Systems\Assets\Pictures\acceleration.gif)

#### Directional forces

![directional force](D:\NCL\CSC8503\Physical-Systems\Assets\Pictures\directional force.gif)

#### State Machines

Player resurrection control via state machine

![](D:\NCL\CSC8503\Physical-Systems\Assets\Pictures\state machine.gif)

#### Simple Pathfinding

![path finding](D:\NCL\CSC8503\Physical-Systems\Assets\Pictures\path finding.gif)

#### Behaviour Trees

Enemy action paths are controlled using behaviour trees

![behavior trees](D:\NCL\CSC8503\Physical-Systems\Assets\Pictures\behavior trees.png)![behavior trees1](D:\NCL\CSC8503\Physical-Systems\Assets\Pictures\behavior trees1.png)

#### Pushdown Automata && Object View

When an enemy sees a player it goes into tracking behaviour, and when it loses the player it returns to the patrol's behaviour tree. 

The enemy's field of view is determined by calculating whether the angle between the vector from the enemy to the player and the direction of the enemy is less than a set value, and then checking whether there is a field of view obstacle between the enemy and the player by ray collision detection.

![pushdown automata](D:\NCL\CSC8503\Physical-Systems\Assets\Pictures\pushdown automata.gif)

