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

![](https://github.com/FishermanSun666/Physical-Systems/blob/master/Assets/Pictures/gravity.gif)

#### Friction

![](https://github.com/FishermanSun666/Physical-Systems/blob/master/Assets/Pictures/friction.gif)

#### Constraint

![](https://github.com/FishermanSun666/Physical-Systems/blob/master/Assets/Pictures/constrain.gif)

#### Spatial Acceleration

![](https://github.com/FishermanSun666/Physical-Systems/blob/master/Assets/Pictures/acceleration.gif)

#### Directional forces

![directional force](https://github.com/FishermanSun666/Physical-Systems/blob/master/Assets/Pictures/directional%20force.gif)

#### State Machines

Player resurrection control via state machine

![](https://github.com/FishermanSun666/Physical-Systems/blob/master/Assets/Pictures/state%20machine.gif)

#### Simple Pathfinding

![path finding](https://github.com/FishermanSun666/Physical-Systems/blob/master/Assets/Pictures/path%20finding.gif)

#### Behaviour Trees

Enemy action paths are controlled using behaviour trees

![behavior trees](https://github.com/FishermanSun666/Physical-Systems/blob/master/Assets/Pictures/behavior%20trees.png)
![behavior%20trees1](https://github.com/FishermanSun666/Physical-Systems/blob/master/Assets/Pictures/behavior%20trees1.png)

#### Pushdown Automata && Object View

When an enemy sees a player it goes into tracking behaviour, and when it loses the player it returns to the patrol's behaviour tree. 

The enemy's field of view is determined by calculating whether the angle between the vector from the enemy to the player and the direction of the enemy is less than a set value, and then checking whether there is a field of view obstacle between the enemy and the player by ray collision detection.

![pushdown automata](https://github.com/FishermanSun666/Physical-Systems/blob/master/Assets/Pictures/pushdown%20automata.gif)

