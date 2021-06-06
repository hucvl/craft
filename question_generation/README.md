# CRAFT: A Benchmark for Causal Reasoning About Forces and inTeractions

## Objects

### Shapes

- Cube
- Triangle
- Circle

### Sizes

- Small
- Large

### Colors

- Gray
- Red
- Blue
- Green
- Brown
- Purple
- Cyan
- Yellow
- Black (Only static objects are black, and they cannot be covered by any other color.)

### Static Scene Elements

- Ramp
- Platform
- Button
- Basket
- Left Wall
- Right Wall
- Ground

## Events

- Start Event (SE)
- End Event (EE)
- Collision Event (CE)
- Start Touching Event (STE)
- End Touching Event (ETE)
- Basket En Up Event (BEUE)

## Input and Output Data Types of Functional Modules

- Object: A dictionary holding static and dynamic properties of an object at a time step
- ObjectSet: A list of unique objects
- ObjectSetList: A list of ObjectSet
- Event: A dictionary holding information of a specific event: id, type, time step, participating objects
- EventSet: A list of unique events
- EventSetList: A list of EventSet
- Color: A tag indicating the color of an object
- Shape: A tag indicating the shape of an object 
- Integer: An integer type
- Bool: A boolean type
- BoolList: A list of Bool

## Side Inputs

### Object Side Inputs

- **Z**: Size
- **C**: Color
- **S**: Shape

### Synonmys

#### Noun and Adjective Synonyms

"thing": "thing", "object"</br>
"sphere": "sphere", "ball"</br>
"cube": "cube", "block"</br>
"large": "large", "big"</br>
"small": "small", "tiny"</br>
"ground": "ground", "floor"</br>
"basket": "basket", "container", "bucket"</br>

#### Verb Synonyms

"prevent": "prevent", "keep", "hold", "block", "hinder"</br>
"prevents": "prevents", "keeps", "holds", "blocks", "hinders"</br>
"prevented": "prevented", "kept", "held", "blocked"</br>
"enable": "enable", "help", "allow"</br>
"enables": "enables", "helps", "allows"</br>
"cause": "cause", "stimulate", "trigger"</br>
"causes": "causes", "stimulates", "triggers"</br>
"enter": "enter", "go into", "get into", "end up in", "fall into"</br>
"entering": "entering", "going into", "getting into", "ending up in", "falling into"</br>
"enters": "enters", "goes into", "gets into", "ends up in", "falls into"</br>
"fall to": "fall to", "hit"</br>
"falling to": "falling to", "hitting"</br>
"falls to": "falls to", "hits"</br>

## Functional Modules

| Name  | Description  | Input Types  | Output Types  |
|---|---|---|---|
| SceneAtStart  | Returns all object properties at the start of the simulation  | None | ObjectSet |
| SceneAtEnd | Returns all object properties at the end of the simulation | None  | ObjectSet |
| StartSceneStep  | Returns 0  | None | Integer |
| EndSceneStep  | Returns -1  | None | Integer |
| Intersect  | Intersects two sets of objects | ObjectSet, ObjectSet | ObjectSet |
| IntersectList  | Intersects an object set with all object sets in a list of ObjectSet | ObjectSetList, ObjectSet | ObjectSetList |
| Events  | Returns all events between video start and end  | None  | EventSet  |
| FilterColor  | Returns objects from input list which has the color of input color  | ObjectSet, Color  | ObjectSet  |
| FilterShape  | Returns objects from input list which has the shape of input shape  | ObjectSet, Shape  | ObjectSet  |
| FilterCollision  | Returns collision events from the input list | EventSet | EventSet |
| FilterCollisionWithDynamics  | Returns collision events including only dynamic objects from the input list | EventSet | EventSet |
| FilterCollideGround | Returns collision events including the ground from the input list | EventSet | EventSet |
| FilterCollideGroundList | Returns list of collision event sets including the ground from a list of event sets | EventSetList | EventSetList |
| FilterCollideBasket | Returns collision events including the basket from the input list | EventSet | EventSet |
| FilterCollideBasketList | Returns list collision event sets including the basket from a list of event sets | EventSetList | EventSetList |
| FilterEnterContainer | Returns container end up events from the input list | EventSet | EventSet |
| FilterEnterContainerList | Returns list of container end up event sets from a list of event sets | EventSetList | EventSetList |
| FilterBefore  | Returns events from the input list that happened before input event  | EventSet, Event  | EventSet  |
| FilterAfter  | Returns events from the input list that happened after input event  | EventSet, Event  | EventSet  |
| IsBefore  | Returns whether the first event happened before the second event  | Event, Event  | Bool  |
| IsAfter  | Returns whether the first event happened after the second event  | Event, Event  | Bool  |
| FilterMoving  | Returns objects if they are moving at step specified by an Integer   | ObjectSet, Integer  | ObjectSet  |
| FilterStationary  | Returns objects if they are stationary at step specified by an Integer   | ObjectSet, Integer  | ObjectSet  |
| FilterFirst  | Returns the first event from the input list  | EventSet  | Event  |
| FilterLast  | Returns the last event from the input list  | EventSet  | Event  |
| FilterUnique  | Returns unique object from input list with possible side inputs Size, Color, Shape  | Objects, ObjectSideInputs | Object  |
| Unique  | Returns the single object from the input list, if list has more than one elements returns INVALID  | Objects | Object  |
| EventPartner (CE, STE, ETE) | Returns object from the object list of the input event which is not input object  | Event, Object  | Object  |
| QueryColor  | Returns the color of the input object  | Object  | Color  |
| QueryShape  | Returns the shape of the input object  | Object  | Shape  |
| Count  | Returns size of the input list  | ObjectSet, EventSet  | Integer  |
| Exist  | Returns true if the input list is not empty  | ObjectSet, EventSet  | Bool  |
| ExistList  | Applies Exist to each item in input list returning a list of Bool | ObjectSetList | BoolList  |
| AnyFalse  | Returns true if there is at least one false in a bool list | BoolList  | Bool |
| AnyTrue  | Returns true if there is at least one true in a bool list | BoolList  | Bool |
| FilterObjectsFromEvents  | Returns objects from events | EventSet  | ObjectSet  |
| FilterObjectsFromEventsList  | Returns list object sets from a list of event sets | EventSetList  | ObjectSetList  |
| GetCounterfactEvents  | Returns event list if a specific object is removed from the scene   | Object  | EventSet  |
| GetCounterfactEventsList  | Returns event list for all objects in an object set | ObjectSet  | EventSetList  |
| FilterDynamic  | Returns dynamic objects from an object set   | ObjectSet  | ObjectSet  |
| AsList  | Returns single elemen object set created with a specific object | Object | ObjectSet  |

## Questions

### Structures Provided to Question Generation Engine

- Scene information at start (SceneAtStart): Holds objects' static and dynamic information at start of the video, color, position, shape, velocity etc.
- Scene information at end (SceneAtEnd): Holds objects' static and dynamic information at end of the video, color, position, shape, velocity etc.
- Causal graph (CausalGraph): Graph constructed by events of objects as nodes. All events causing a specific event are the ancestors of that event.

### All Tasks

| Task  |  Category |
|---|---|
| 1. "What color is the object the **Z** **C** **S** first collides with?", "What color is the first object that collided with the **Z** **C** **S**?", "What is the color of the object the **Z** **C** **S** first collides with?", "What is the color of the first object that collided with the **Z** **C** **S**?" | Descriptive |
| 2. "What shape is the object the **Z** **C** **S** first collides with?", "What shape is the first object that collided with the **Z** **C** **S**?", "What is the shape of the object the **Z** **C** **S** first collides with?", "What is the shape of the first object that collided with the **Z** **C** **S**?" | Descriptive |
| 3. "What color is the object the **Z** **C** **S** last collides with?", "What color is the last object that collided with the **Z** **C** **S**?", "What is the color of the object the **Z** **C** **S** last collides with?", "What is the color of the last object that collided with the **Z** **C** **S**?" | Descriptive |
| 4. "What shape is the object the **Z** **C** **S** last collides with?", "What shape is the last object that collided with the **Z** **C** **S**?", "What is the shape of the object the **Z** **C** **S** last collides with?", "What is the shape of the last object that collided with the **Z** **C** **S**?" | Descriptive |
| 5. "How many **S**s are moving when the video ends?", "How many **S**s are in motion at the end of the video?" | Descriptive |
| 6. "How many **C** objects are moving when the video ends?", "How many **C** objects are in motion at the end of the video?" | Descriptive |
| 7. "How many **Z** objects are moving when the video ends?", "How many **Z** objects are in motion at the end of the video?" | Descriptive |
| 8. "How many objects are moving when the video ends?", "How many objects are in motion at the end of the video?" | Descriptive |
| 9. "How many **S**s **enter** the **basket**?" | Descriptive|
| 10. "How many **C** objects **enter** the **basket**?" | Descriptive |
| 11. "How many **Z** objects **enter** the **basket**?" | Descriptive |
| 12. "How many objects **enter** the **basket**?" | Descriptive |
| 13. "How many **S**s **fall to** the **ground**?" | Descriptive |
| 14. "How many **C** objects **fall to** the **ground**?" | Descriptive |
| 15. "How many **Z** objects **fall to** the **ground**?" | Descriptive |
| 16. "How many objects **fall to** the **ground**?" | Descriptive |
| 17. "How many objects **enter** the **basket** after the **Z** **C** **S** **enters** the **basket**?" | Descriptive |
| 18. "How many objects **enter** the **basket** before the **Z** **C** **S** **enters** the **basket**?" | Descriptive |
| 19. "How many objects **fall to** the **ground** after the **Z** **C** **S** **falls** to the **ground**?" | Descriptive |
| 20. "How many objects **fall to** the **ground** before the **Z** **C** **S** **falls** to the **ground**?" | Descriptive |
| 21. "After **entering** the **basket**, does the **Z** **C** **S** collide with other objects?" | Descriptive |
| 22. "Before **entering** the **basket**, does the **Z** **C** **S** collide with other objects?" | Descriptive |
| 23. "After **falling to** the **ground**, does the **Z** **C** **S** collide with other objects?" | Descriptive |
| 24. "Before **falling to** the **ground**, does the **Z** **C** **S** collide with other objects?" | Descriptive |
| 25. "Are there any collisions between objects after the **Z** **C** **S** **enters** the **basket**?" | Descriptive |
| 26. "Are there any collisions between objects before the **Z** **C** **S** **enters** the **basket**?" | Descriptive |
| 27. "Are there any collisions between objects after the **Z** **C** **S** **falls to** the **ground**?" | Descriptive |
| 28. "Are there any collisions between objects before the **Z** **C** **S** **falls to** the **ground**?" | Descriptive |
| 29. "Does the **Z** **C** **S** **enter** the **basket** before the **Z2** **C2** **S2** does?" | Descriptive |
| 30. "Does the **Z** **C** **S** **fall to** the **ground** before the **Z2** **C2** **S2** does?" | Descriptive |
| 31. "Will the **Z2** **C2** **S2** **enter** the **basket** if the **Z** **C** **S** is removed?", "If the **Z** **C** **S** is removed, will the **Z2** **C2** **S2** **enter** the **basket**?" | Counterfactual |
| 32. "Will the **Z2** **C2** **S2** **fall to** the **ground** if the **Z** **C** **S** is removed?", "If the **Z** **C** **S** is removed, will the **Z2** **C2** **S2** **fall to** the **ground**?" | Counterfactual |
| 33. "How many objects **enter** the **basket** if the **Z** **C** **S** is removed?", "If the **Z** **C** **S** is removed, how many objects **enter** the **basket**?" | Counterfactual |
| 34. "How many objects **fall to** the **ground** if the **Z** **C** **S** is removed?", "If the **Z** **C** **S** is removed, how many objects **fall to** the **ground**?" | Counterfactual |
| 35. "Will the **Z** **C** **S** **enter** the **basket** if any of the other objects are removed?", "If any of the other objects are removed, will the **Z** **C** **S** **enter** the **basket**?" | Counterfactual |
| 36. "Will the **Z** **C** **S** **fall to** the **ground** if any of the other objects are removed?", "If any of other the objects are removed, will the **Z** **C** **S** **fall to** the **ground**?" | Counterfactual |
| 37. "Does the **Z** **C** **S** **enable** the **Z2** **C2** **S2** to **fall to** the **ground**?", "Does the **Z** **C** **S** **enable** the collision between the **Z2** **C2** **S2** and the **ground**?", "There is a **Z** **C** **S**, does it **enable** **Z2** **C2** **S2** to **fall to** the **ground**?" | Enable |
| 38. "Does the **Z** **C** **S** **enable** the **Z2** **C2** **S2** to **enter** the **basket**?", "There is a **Z** **C** **S**, does it **enable** the **Z2** **C2** **S2** to **enter** the **basket**?" | Enable |
| 39. "How many objects does the **Z** **C** **S** **enable** to **fall to** the **ground**?", "What is the number of objects that the **Z** **C** **S** **enables** to **fall to** the **ground**?" | Enable |
| 40. "How many objects does the **Z** **C** **S** **enable** to **enter** the **basket**?", "What is the number of objects that the **Z** **C** **S** **enables** to **enter** the **basket**?" | Enable |
| 41. "Does the **Z** **C** **S** **cause** the **Z2** **C2** **S2** to **fall to** the **ground**?", "Does the **Z** **C** **S** **cause** the collision between the **Z2** **C2** **S2** and the **ground**?", "There is a **Z** **C** **S**, does it **cause** the **Z2** **C2** **S2** to **fall to** the **ground**?",       "Does the **Z** **C** **S** lead to the collision between the **Z2** **C2** **S2** and the **ground**?", "Does the **Z** **C** **S** lead to the **Z2** **C2** **S2** **falling to** the **ground**?" | Cause |
| 42. "Does the **Z** **C** **S** **cause** the **Z2** **C2** **S2** to **enter** the **basket**?", "There is a **Z** **C** **S**, does it **cause** the **Z2** **C2** **S2** to **enter** the **basket**?", "Does the **Z** **C** **S** lead to the **Z2** **C2** **S2** **entering** the **basket**?" | Cause |
| 43. "How many objects does the **Z** **C** **S** **cause** to **fall to** the **ground**?", "What is the number of objects that the **Z** **C** **S** **causes** to **fall to** the **ground**?" | Cause |
| 44. "How many objects does the **Z** **C** **S** **cause** to **enter** the **basket**?", "What is the number of objects that the **Z** **C** **S** **causes** to **enter** the **basket**?" | Cause |
| 45. "Does the **Z** **C** **S** **prevent** the **Z2** **C2** **S2** from **falling to** the **ground**?", "There is a **Z** **C** **S**, does it **prevent** the **Z2** **C2** **S2** from **falling to** the **ground**?", "Is the **Z2** **C2** **S2** is **prevented** by the **Z** **C** **S** from **falling to** the **ground**?" | Prevent |
| 46. "Does the **Z** **C** **S** **prevent** the **Z2** **C2** **S2** from **entering** the **basket**?", "There is a **Z** **C** **S**, does it **prevent** the **Z2** **C2** **S2** from **entering** the **basket**?", "Is the **Z2** **C2** **S2** is **prevented** by the **Z** **C** **S** from **entering** the **basket**?" | Prevent |
| 47. "How many objects does the **Z** **C** **S** **prevent** from **falling to** the **ground**?", "What is the number of objects that the **Z** **C** **S** **prevents** from **falling to** the **ground**?", "How many objects are **prevented** by the **Z** **C** **S** from **falling to** the **ground**?", "What is the number of objects that are **prevented** by the **Z** **C** **S** from **falling to** the **ground**?" | Prevent |
| 48. "How many objects does the **Z** **C** **S** **prevent** from **entering** the **basket**?", "What is the number of objects that the **Z** **C** **S** **prevents** from **entering** the **basket**?", "How many objects are **prevented** by the **Z** **C** **S** from **entering** the **basket**?", "What is the number of objects that are **prevented** by the **Z** **C** **S** from **entering** the **basket**?" | Prevent |

### Relationships between Enable, Cause and Prevent Tasks

In order to have better understanding about the differences between the Enable, Cause, and Prevent tasks, one should understand the notion of "intention" in our environments. We identify the intention in a simulation by looking at the initial linear velocity of the corresponding object. If the magnitude of the velocity is greater than zero, then the object **is intended to** do the task specified in the question text, such as entering the basket or colliding with the ground. If the magnitude of the velocity is zero, then the object **is not intended to** do the task, even if there is an external force, such as gravity, upon it at the start of the simulation. Therefore, an affector object or event can only **enable** a patient object to do the task if the patient object is intended to do it but fails without the affector. Similarly, an affector object or event can only **cause** a patient object to do the task if the patient object is **not** intended to do it. Furthermore, an affector object or event can only **prevent** a patient object **not** to do the task if the patient object is intended to do it and succeeds without the affector.

Here are some examples to make these more clear:

![](/dataset/examples/ExampleScene12.gif)

**"Does the small purple triangle cause the tiny green triangle to get into the bucket?"**

The answer to this question is true since the green tiny triangle is not intended to enter the bucket (its initial x and y velocties are 0) and without the purple object, it does not enter the bucket.

**Does the large cyan triangle enable the tiny purple triangle to hit the bottom?**

The answer to this question is true since tiny purple triangle is intended to hit the bottom (its initial x and/or y velocities are not 0s) and without the large cyan triangle, the blue box would not collide with the purple triangle leading it to hit the bottom.

If we switch **cause** and **enable** keywords in these questions, then the answers become **false** because of the patient object intentions.

Similarly,

**Does the big cyan triangle hold the big blue block from going into the basket?**

The answer to this question is true since big blue block is intended to enter the basket, and without the cyan triangle, it succeeds to do so.

### Dataset Statistics

#### Answer Distributions for Different Simulations

| All | SID=1 | SID=2|
|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Answer%20frequencies.png)| ![](/dataset/outputs/stats/Dataset_v230920/Answer%20frequencies%20for%20Simulation%20ID%3D1.png)| ![](/dataset/outputs/stats/Dataset_v230920/Answer%20frequencies%20for%20Simulation%20ID%3D2.png)|

| SID=3 | SID=4 | SID=5|
|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Answer%20frequencies%20for%20Simulation%20ID%3D3.png)| ![](/dataset/outputs/stats/Dataset_v230920/Answer%20frequencies%20for%20Simulation%20ID%3D4.png)| ![](/dataset/outputs/stats/Dataset_v230920/Answer%20frequencies%20for%20Simulation%20ID%3D5.png)|

| SID=6 | SID=7 | SID=8|
|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Answer%20frequencies%20for%20Simulation%20ID%3D6.png)| ![](/dataset/outputs/stats/Dataset_v230920/Answer%20frequencies%20for%20Simulation%20ID%3D7.png)| ![](/dataset/outputs/stats/Dataset_v230920/Answer%20frequencies%20for%20Simulation%20ID%3D8.png)|

| SID=9 | SID=10 |
|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Answer%20frequencies%20for%20Simulation%20ID%3D9.png)| ![](/dataset/outputs/stats/Dataset_v230920/Answer%20frequencies%20for%20Simulation%20ID%3D10.png)|

#### Answer Distributions for Different Tasks

| TID=cause_0 | TID=cause_1 | TID=cause_2 |
|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Dcause_0.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Dcause_1.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Dcause_2.png)|

| TID=cause_3 | TID=cause_4 | TID=cause_5 |
|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Dcause_3.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Dcause_4.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Dcause_5.png)|

| TID=counterfactual_0 | TID=counterfactual_1 | TID=counterfactual_2 |
|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Dcounterfactual_0.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Dcounterfactual_1.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Dcounterfactual_2.png)|

| TID=counterfactual_3 | TID=counterfactual_4 | TID=counterfactual_5 |
|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Dcounterfactual_3.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Dcounterfactual_4.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Dcounterfactual_5.png)|

| TID=counterfactual_6 | TID=counterfactual_7 | TID=counterfactual_8 |
|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Dcounterfactual_6.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Dcounterfactual_7.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Dcounterfactual_8.png)|

| TID=descriptive_0 | TID=descriptive_1 | TID=descriptive_2 |
|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_0.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_1.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_2.png)|

| TID=descriptive_3 | TID=descriptive_4 | TID=descriptive_5 |
|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_3.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_4.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_5.png)|

| TID=descriptive_6 | TID=descriptive_7 | TID=descriptive_8 |
|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_6.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_7.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_8.png)|

| TID=descriptive_9 | TID=descriptive_10 | TID=descriptive_11 |
|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_9.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_10.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_11.png)|

| TID=descriptive_12 | TID=descriptive_13 | TID=descriptive_14 |
|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_12.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_13.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_14.png)|

| TID=descriptive_15 | TID=descriptive_16 | TID=descriptive_17 |
|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_15.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_16.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_17.png)|

| TID=descriptive_18 | TID=descriptive_19 | TID=descriptive_20 |
|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_18.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_19.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_20.png)|

| TID=descriptive_21 | TID=descriptive_22 | TID=descriptive_23 |
|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_21.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_22.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_23.png)|

| TID=descriptive_24 | TID=descriptive_25 | TID=descriptive_26 |
|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_24.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_25.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_26.png)|

| TID=descriptive_27 | TID=descriptive_28 | TID=descriptive_29 |
|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_27.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_28.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_29.png)|

| TID=descriptive_30 | TID=descriptive_31 | TID=descriptive_32 |
|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_30.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_31.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_32.png)|

| TID=descriptive_33 | TID=descriptive_34 | TID=descriptive_35 |
|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_33.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_34.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_35.png)|

| TID=descriptive_36 | TID=descriptive_37 |
|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_36.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Ddescriptive_37.png)|

| TID=enable_0 | TID=enable_1 | TID=enable_2 |
|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Denable_0.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Denable_1.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Denable_2.png)|

| TID=enable_3 | TID=enable_4 | TID=enable_5 |
|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Denable_3.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Denable_4.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Denable_5.png)|

| TID=prevent_0 | TID=prevent_1 | TID=prevent_2 |
|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Dprevent_0.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Dprevent_1.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Dprevent_2.png)|

| TID=prevent_3 | TID=prevent_4 | TID=prevent_5 |
|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Dprevent_3.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Dprevent_4.png)| ![](/dataset/outputs/stats/Dataset_v230920/Template%20ID%3Dprevent_5.png)|


#### Answer Distributions for Simulation and Task Pairs

| SID=1, TID=cause_0| SID=1, TID=cause_1| SID=1, TID=cause_2 | SID=1, TID=cause_3|SID=1, TID=cause_4 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Dcause_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Dcause_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Dcause_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Dcause_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Dcause_4.png)|

| SID=1, TID=cause_5| SID=1, TID=counterfactual_0| SID=1, TID=counterfactual_1 | SID=1, TID=counterfactual_2|SID=1, TID=counterfactual_3 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Dcause_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Dcounterfactual_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Dcounterfactual_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Dcounterfactual_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Dcounterfactual_3.png)|

| SID=1, TID=counterfactual_4| SID=1, TID=counterfactual_5| SID=1, TID=counterfactual_6 | SID=1, TID=counterfactual_7|SID=1, TID=counterfactual_8 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Dcounterfactual_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Dcounterfactual_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Dcounterfactual_6.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Dcounterfactual_7.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Dcounterfactual_8.png)|

| SID=1, TID=descriptive_0| SID=1, TID=descriptive_1| SID=1, TID=descriptive_10 | SID=1, TID=descriptive_11|SID=1, TID=descriptive_12 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_10.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_11.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_12.png)|

| SID=1, TID=descriptive_13| SID=1, TID=descriptive_14| SID=1, TID=descriptive_15 | SID=1, TID=descriptive_16|SID=1, TID=descriptive_17 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_13.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_14.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_15.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_16.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_17.png)|

| SID=1, TID=descriptive_18| SID=1, TID=descriptive_19| SID=1, TID=descriptive_2 | SID=1, TID=descriptive_20|SID=1, TID=descriptive_21 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_18.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_19.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_20.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_21.png)|

| SID=1, TID=descriptive_22| SID=1, TID=descriptive_23| SID=1, TID=descriptive_24 | SID=1, TID=descriptive_25|SID=1, TID=descriptive_26 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_22.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_23.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_24.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_25.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_26.png)|

| SID=1, TID=descriptive_27| SID=1, TID=descriptive_28| SID=1, TID=descriptive_29 | SID=1, TID=descriptive_3|SID=1, TID=descriptive_30 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_27.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_28.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_29.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_30.png)|

| SID=1, TID=descriptive_31| SID=1, TID=descriptive_32| SID=1, TID=descriptive_33 | SID=1, TID=descriptive_34|SID=1, TID=descriptive_35 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_31.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_32.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_33.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_34.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_35.png)|

| SID=1, TID=descriptive_36| SID=1, TID=descriptive_37| SID=1, TID=descriptive_4 | SID=1, TID=descriptive_5|SID=1, TID=descriptive_6 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_36.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_37.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_6.png)|

| SID=1, TID=descriptive_7| SID=1, TID=descriptive_8| SID=1, TID=descriptive_9 | SID=1, TID=enable_0|SID=1, TID=enable_1 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_7.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_8.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Ddescriptive_9.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Denable_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Denable_1.png)|

| SID=1, TID=enable_2| SID=1, TID=enable_3| SID=1, TID=enable_4 | SID=1, TID=enable_5|SID=1, TID=prevent_0 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Denable_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Denable_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Denable_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Denable_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Dprevent_0.png)|

| SID=1, TID=prevent_1| SID=1, TID=prevent_2| SID=1, TID=prevent_3 | SID=1, TID=prevent_4|SID=1, TID=prevent_5 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Dprevent_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Dprevent_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Dprevent_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Dprevent_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D1-TID%3Dprevent_5.png)|

| SID=2, TID=cause_0| SID=2, TID=cause_1| SID=2, TID=cause_2 | SID=2, TID=cause_3|SID=2, TID=cause_4 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Dcause_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Dcause_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Dcause_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Dcause_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Dcause_4.png)|

| SID=2, TID=cause_5| SID=2, TID=counterfactual_0| SID=2, TID=counterfactual_1 | SID=2, TID=counterfactual_2|SID=2, TID=counterfactual_3 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Dcause_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Dcounterfactual_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Dcounterfactual_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Dcounterfactual_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Dcounterfactual_3.png)|

| SID=2, TID=counterfactual_4| SID=2, TID=counterfactual_5| SID=2, TID=counterfactual_6 | SID=2, TID=counterfactual_7|SID=2, TID=counterfactual_8 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Dcounterfactual_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Dcounterfactual_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Dcounterfactual_6.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Dcounterfactual_7.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Dcounterfactual_8.png)|

| SID=2, TID=descriptive_0| SID=2, TID=descriptive_1| SID=2, TID=descriptive_10 | SID=2, TID=descriptive_11|SID=2, TID=descriptive_12 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_10.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_11.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_12.png)|

| SID=2, TID=descriptive_13| SID=2, TID=descriptive_14| SID=2, TID=descriptive_15 | SID=2, TID=descriptive_16|SID=2, TID=descriptive_17 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_13.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_14.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_15.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_16.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_17.png)|

| SID=2, TID=descriptive_18| SID=2, TID=descriptive_19| SID=2, TID=descriptive_2 | SID=2, TID=descriptive_20|SID=2, TID=descriptive_21 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_18.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_19.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_20.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_21.png)|

| SID=2, TID=descriptive_22| SID=2, TID=descriptive_23| SID=2, TID=descriptive_24 | SID=2, TID=descriptive_25|SID=2, TID=descriptive_26 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_22.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_23.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_24.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_25.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_26.png)|

| SID=2, TID=descriptive_27| SID=2, TID=descriptive_28| SID=2, TID=descriptive_29 | SID=2, TID=descriptive_3|SID=2, TID=descriptive_30 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_27.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_28.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_29.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_30.png)|

| SID=2, TID=descriptive_31| SID=2, TID=descriptive_32| SID=2, TID=descriptive_33 | SID=2, TID=descriptive_34|SID=2, TID=descriptive_35 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_31.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_32.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_33.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_34.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_35.png)|

| SID=2, TID=descriptive_36| SID=2, TID=descriptive_37| SID=2, TID=descriptive_4 | SID=2, TID=descriptive_5|SID=2, TID=descriptive_6 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_36.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_37.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_6.png)|

| SID=2, TID=descriptive_7| SID=2, TID=descriptive_8| SID=2, TID=descriptive_9 | SID=2, TID=enable_0|SID=2, TID=enable_1 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_7.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_8.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Ddescriptive_9.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Denable_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Denable_1.png)|

| SID=2, TID=enable_2| SID=2, TID=enable_3| SID=2, TID=enable_4 | SID=2, TID=enable_5|SID=2, TID=prevent_0 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Denable_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Denable_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Denable_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Denable_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Dprevent_0.png)|

| SID=2, TID=prevent_1| SID=2, TID=prevent_2| SID=2, TID=prevent_3 | SID=2, TID=prevent_4|SID=2, TID=prevent_5 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Dprevent_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Dprevent_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Dprevent_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Dprevent_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D2-TID%3Dprevent_5.png)|

| SID=3, TID=cause_0| SID=3, TID=cause_1| SID=3, TID=cause_2 | SID=3, TID=cause_3|SID=3, TID=cause_4 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Dcause_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Dcause_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Dcause_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Dcause_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Dcause_4.png)|

| SID=3, TID=cause_5| SID=3, TID=counterfactual_0| SID=3, TID=counterfactual_1 | SID=3, TID=counterfactual_2|SID=3, TID=counterfactual_3 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Dcause_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Dcounterfactual_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Dcounterfactual_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Dcounterfactual_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Dcounterfactual_3.png)|

| SID=3, TID=counterfactual_4| SID=3, TID=counterfactual_5| SID=3, TID=counterfactual_6 | SID=3, TID=counterfactual_7|SID=3, TID=counterfactual_8 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Dcounterfactual_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Dcounterfactual_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Dcounterfactual_6.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Dcounterfactual_7.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Dcounterfactual_8.png)|

| SID=3, TID=descriptive_0| SID=3, TID=descriptive_1| SID=3, TID=descriptive_10 | SID=3, TID=descriptive_11|SID=3, TID=descriptive_12 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_10.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_11.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_12.png)|

| SID=3, TID=descriptive_13| SID=3, TID=descriptive_14| SID=3, TID=descriptive_15 | SID=3, TID=descriptive_16|SID=3, TID=descriptive_17 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_13.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_14.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_15.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_16.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_17.png)|

| SID=3, TID=descriptive_18| SID=3, TID=descriptive_19| SID=3, TID=descriptive_2 | SID=3, TID=descriptive_20|SID=3, TID=descriptive_21 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_18.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_19.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_20.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_21.png)|

| SID=3, TID=descriptive_22| SID=3, TID=descriptive_23| SID=3, TID=descriptive_24 | SID=3, TID=descriptive_25|SID=3, TID=descriptive_26 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_22.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_23.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_24.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_25.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_26.png)|

| SID=3, TID=descriptive_27| SID=3, TID=descriptive_28| SID=3, TID=descriptive_29 | SID=3, TID=descriptive_3|SID=3, TID=descriptive_30 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_27.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_28.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_29.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_30.png)|

| SID=3, TID=descriptive_31| SID=3, TID=descriptive_32| SID=3, TID=descriptive_33 | SID=3, TID=descriptive_34|SID=3, TID=descriptive_35 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_31.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_32.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_33.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_34.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_35.png)|

| SID=3, TID=descriptive_36| SID=3, TID=descriptive_37| SID=3, TID=descriptive_4 | SID=3, TID=descriptive_5|SID=3, TID=descriptive_6 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_36.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_37.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_6.png)|

| SID=3, TID=descriptive_7| SID=3, TID=descriptive_8| SID=3, TID=descriptive_9 | SID=3, TID=enable_0|SID=3, TID=enable_1 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_7.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_8.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Ddescriptive_9.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Denable_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Denable_1.png)|

| SID=3, TID=enable_2| SID=3, TID=enable_3| SID=3, TID=enable_4 | SID=3, TID=enable_5|SID=3, TID=prevent_0 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Denable_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Denable_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Denable_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Denable_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Dprevent_0.png)|

| SID=3, TID=prevent_1| SID=3, TID=prevent_2| SID=3, TID=prevent_3 | SID=3, TID=prevent_4|SID=3, TID=prevent_5 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Dprevent_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Dprevent_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Dprevent_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Dprevent_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D3-TID%3Dprevent_5.png)|

| SID=4, TID=cause_0| SID=4, TID=cause_1| SID=4, TID=cause_2 | SID=4, TID=cause_3|SID=4, TID=cause_4 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Dcause_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Dcause_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Dcause_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Dcause_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Dcause_4.png)|

| SID=4, TID=cause_5| SID=4, TID=counterfactual_0| SID=4, TID=counterfactual_1 | SID=4, TID=counterfactual_2|SID=4, TID=counterfactual_3 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Dcause_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Dcounterfactual_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Dcounterfactual_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Dcounterfactual_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Dcounterfactual_3.png)|

| SID=4, TID=counterfactual_4| SID=4, TID=counterfactual_5| SID=4, TID=counterfactual_6 | SID=4, TID=counterfactual_7|SID=4, TID=counterfactual_8 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Dcounterfactual_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Dcounterfactual_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Dcounterfactual_6.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Dcounterfactual_7.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Dcounterfactual_8.png)|

| SID=4, TID=descriptive_0| SID=4, TID=descriptive_1| SID=4, TID=descriptive_10 | SID=4, TID=descriptive_11|SID=4, TID=descriptive_12 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_10.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_11.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_12.png)|

| SID=4, TID=descriptive_13| SID=4, TID=descriptive_14| SID=4, TID=descriptive_15 | SID=4, TID=descriptive_16|SID=4, TID=descriptive_17 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_13.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_14.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_15.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_16.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_17.png)|

| SID=4, TID=descriptive_18| SID=4, TID=descriptive_19| SID=4, TID=descriptive_2 | SID=4, TID=descriptive_20|SID=4, TID=descriptive_21 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_18.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_19.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_20.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_21.png)|

| SID=4, TID=descriptive_22| SID=4, TID=descriptive_23| SID=4, TID=descriptive_24 | SID=4, TID=descriptive_25|SID=4, TID=descriptive_26 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_22.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_23.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_24.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_25.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_26.png)|

| SID=4, TID=descriptive_27| SID=4, TID=descriptive_28| SID=4, TID=descriptive_29 | SID=4, TID=descriptive_3|SID=4, TID=descriptive_30 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_27.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_28.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_29.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_30.png)|

| SID=4, TID=descriptive_31| SID=4, TID=descriptive_32| SID=4, TID=descriptive_33 | SID=4, TID=descriptive_34|SID=4, TID=descriptive_35 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_31.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_32.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_33.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_34.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_35.png)|

| SID=4, TID=descriptive_36| SID=4, TID=descriptive_37| SID=4, TID=descriptive_4 | SID=4, TID=descriptive_5|SID=4, TID=descriptive_6 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_36.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_37.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_6.png)|

| SID=4, TID=descriptive_7| SID=4, TID=descriptive_8| SID=4, TID=descriptive_9 | SID=4, TID=enable_0|SID=4, TID=enable_1 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_7.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_8.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Ddescriptive_9.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Denable_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Denable_1.png)|

| SID=4, TID=enable_2| SID=4, TID=enable_3| SID=4, TID=enable_4 | SID=4, TID=enable_5|SID=4, TID=prevent_0 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Denable_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Denable_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Denable_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Denable_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Dprevent_0.png)|

| SID=4, TID=prevent_1| SID=4, TID=prevent_2| SID=4, TID=prevent_3 | SID=4, TID=prevent_4|SID=4, TID=prevent_5 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Dprevent_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Dprevent_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Dprevent_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Dprevent_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D4-TID%3Dprevent_5.png)|

| SID=5, TID=cause_0| SID=5, TID=cause_1| SID=5, TID=cause_2 | SID=5, TID=cause_3|SID=5, TID=cause_4 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Dcause_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Dcause_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Dcause_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Dcause_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Dcause_4.png)|

| SID=5, TID=cause_5| SID=5, TID=counterfactual_0| SID=5, TID=counterfactual_1 | SID=5, TID=counterfactual_2|SID=5, TID=counterfactual_3 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Dcause_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Dcounterfactual_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Dcounterfactual_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Dcounterfactual_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Dcounterfactual_3.png)|

| SID=5, TID=counterfactual_4| SID=5, TID=counterfactual_5| SID=5, TID=counterfactual_6 | SID=5, TID=counterfactual_7|SID=5, TID=counterfactual_8 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Dcounterfactual_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Dcounterfactual_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Dcounterfactual_6.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Dcounterfactual_7.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Dcounterfactual_8.png)|

| SID=5, TID=descriptive_0| SID=5, TID=descriptive_1| SID=5, TID=descriptive_10 | SID=5, TID=descriptive_11|SID=5, TID=descriptive_12 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_10.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_11.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_12.png)|

| SID=5, TID=descriptive_13| SID=5, TID=descriptive_14| SID=5, TID=descriptive_15 | SID=5, TID=descriptive_16|SID=5, TID=descriptive_17 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_13.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_14.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_15.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_16.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_17.png)|

| SID=5, TID=descriptive_18| SID=5, TID=descriptive_19| SID=5, TID=descriptive_2 | SID=5, TID=descriptive_20|SID=5, TID=descriptive_21 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_18.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_19.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_20.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_21.png)|

| SID=5, TID=descriptive_22| SID=5, TID=descriptive_23| SID=5, TID=descriptive_24 | SID=5, TID=descriptive_25|SID=5, TID=descriptive_26 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_22.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_23.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_24.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_25.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_26.png)|

| SID=5, TID=descriptive_27| SID=5, TID=descriptive_28| SID=5, TID=descriptive_29 | SID=5, TID=descriptive_3|SID=5, TID=descriptive_30 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_27.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_28.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_29.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_30.png)|

| SID=5, TID=descriptive_31| SID=5, TID=descriptive_32| SID=5, TID=descriptive_33 | SID=5, TID=descriptive_34|SID=5, TID=descriptive_35 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_31.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_32.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_33.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_34.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_35.png)|

| SID=5, TID=descriptive_36| SID=5, TID=descriptive_37| SID=5, TID=descriptive_4 | SID=5, TID=descriptive_5|SID=5, TID=descriptive_6 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_36.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_37.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_6.png)|

| SID=5, TID=descriptive_7| SID=5, TID=descriptive_8| SID=5, TID=descriptive_9 | SID=5, TID=enable_0|SID=5, TID=enable_1 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_7.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_8.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Ddescriptive_9.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Denable_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Denable_1.png)|

| SID=5, TID=enable_2| SID=5, TID=enable_3| SID=5, TID=enable_4 | SID=5, TID=enable_5|SID=5, TID=prevent_0 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Denable_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Denable_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Denable_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Denable_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Dprevent_0.png)|

| SID=5, TID=prevent_1| SID=5, TID=prevent_2| SID=5, TID=prevent_3 | SID=5, TID=prevent_4|SID=5, TID=prevent_5 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Dprevent_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Dprevent_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Dprevent_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Dprevent_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D5-TID%3Dprevent_5.png)|

| SID=6, TID=cause_0| SID=6, TID=cause_1| SID=6, TID=cause_2 | SID=6, TID=cause_3|SID=6, TID=cause_4 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Dcause_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Dcause_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Dcause_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Dcause_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Dcause_4.png)|

| SID=6, TID=cause_5| SID=6, TID=counterfactual_0| SID=6, TID=counterfactual_1 | SID=6, TID=counterfactual_2|SID=6, TID=counterfactual_3 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Dcause_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Dcounterfactual_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Dcounterfactual_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Dcounterfactual_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Dcounterfactual_3.png)|

| SID=6, TID=counterfactual_4| SID=6, TID=counterfactual_5| SID=6, TID=counterfactual_6 | SID=6, TID=counterfactual_7|SID=6, TID=counterfactual_8 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Dcounterfactual_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Dcounterfactual_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Dcounterfactual_6.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Dcounterfactual_7.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Dcounterfactual_8.png)|

| SID=6, TID=descriptive_0| SID=6, TID=descriptive_1| SID=6, TID=descriptive_10 | SID=6, TID=descriptive_11|SID=6, TID=descriptive_12 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_10.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_11.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_12.png)|

| SID=6, TID=descriptive_13| SID=6, TID=descriptive_14| SID=6, TID=descriptive_15 | SID=6, TID=descriptive_16|SID=6, TID=descriptive_17 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_13.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_14.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_15.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_16.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_17.png)|

| SID=6, TID=descriptive_18| SID=6, TID=descriptive_19| SID=6, TID=descriptive_2 | SID=6, TID=descriptive_20|SID=6, TID=descriptive_21 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_18.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_19.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_20.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_21.png)|

| SID=6, TID=descriptive_22| SID=6, TID=descriptive_23| SID=6, TID=descriptive_24 | SID=6, TID=descriptive_25|SID=6, TID=descriptive_26 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_22.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_23.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_24.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_25.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_26.png)|

| SID=6, TID=descriptive_27| SID=6, TID=descriptive_28| SID=6, TID=descriptive_29 | SID=6, TID=descriptive_3|SID=6, TID=descriptive_30 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_27.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_28.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_29.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_30.png)|

| SID=6, TID=descriptive_31| SID=6, TID=descriptive_32| SID=6, TID=descriptive_33 | SID=6, TID=descriptive_34|SID=6, TID=descriptive_35 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_31.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_32.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_33.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_34.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_35.png)|

| SID=6, TID=descriptive_36| SID=6, TID=descriptive_37| SID=6, TID=descriptive_4 | SID=6, TID=descriptive_5|SID=6, TID=descriptive_6 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_36.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_37.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_6.png)|

| SID=6, TID=descriptive_7| SID=6, TID=descriptive_8| SID=6, TID=descriptive_9 | SID=6, TID=enable_0|SID=6, TID=enable_1 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_7.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_8.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Ddescriptive_9.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Denable_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Denable_1.png)|

| SID=6, TID=enable_2| SID=6, TID=enable_3| SID=6, TID=enable_4 | SID=6, TID=enable_5|SID=6, TID=prevent_0 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Denable_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Denable_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Denable_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Denable_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Dprevent_0.png)|

| SID=6, TID=prevent_1| SID=6, TID=prevent_2| SID=6, TID=prevent_3 | SID=6, TID=prevent_4|SID=6, TID=prevent_5 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Dprevent_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Dprevent_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Dprevent_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Dprevent_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D6-TID%3Dprevent_5.png)|

| SID=7, TID=cause_0| SID=7, TID=cause_1| SID=7, TID=cause_2 | SID=7, TID=cause_3|SID=7, TID=cause_4 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Dcause_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Dcause_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Dcause_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Dcause_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Dcause_4.png)|

| SID=7, TID=cause_5| SID=7, TID=counterfactual_0| SID=7, TID=counterfactual_1 | SID=7, TID=counterfactual_2|SID=7, TID=counterfactual_3 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Dcause_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Dcounterfactual_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Dcounterfactual_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Dcounterfactual_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Dcounterfactual_3.png)|

| SID=7, TID=counterfactual_4| SID=7, TID=counterfactual_5| SID=7, TID=counterfactual_6 | SID=7, TID=counterfactual_7|SID=7, TID=counterfactual_8 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Dcounterfactual_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Dcounterfactual_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Dcounterfactual_6.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Dcounterfactual_7.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Dcounterfactual_8.png)|

| SID=7, TID=descriptive_0| SID=7, TID=descriptive_1| SID=7, TID=descriptive_10 | SID=7, TID=descriptive_11|SID=7, TID=descriptive_12 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_10.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_11.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_12.png)|

| SID=7, TID=descriptive_13| SID=7, TID=descriptive_14| SID=7, TID=descriptive_15 | SID=7, TID=descriptive_16|SID=7, TID=descriptive_17 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_13.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_14.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_15.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_16.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_17.png)|

| SID=7, TID=descriptive_18| SID=7, TID=descriptive_19| SID=7, TID=descriptive_2 | SID=7, TID=descriptive_20|SID=7, TID=descriptive_21 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_18.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_19.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_20.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_21.png)|

| SID=7, TID=descriptive_22| SID=7, TID=descriptive_23| SID=7, TID=descriptive_24 | SID=7, TID=descriptive_25|SID=7, TID=descriptive_26 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_22.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_23.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_24.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_25.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_26.png)|

| SID=7, TID=descriptive_27| SID=7, TID=descriptive_28| SID=7, TID=descriptive_29 | SID=7, TID=descriptive_3|SID=7, TID=descriptive_30 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_27.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_28.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_29.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_30.png)|

| SID=7, TID=descriptive_31| SID=7, TID=descriptive_32| SID=7, TID=descriptive_33 | SID=7, TID=descriptive_34|SID=7, TID=descriptive_35 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_31.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_32.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_33.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_34.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_35.png)|

| SID=7, TID=descriptive_36| SID=7, TID=descriptive_37| SID=7, TID=descriptive_4 | SID=7, TID=descriptive_5|SID=7, TID=descriptive_6 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_36.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_37.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_6.png)|

| SID=7, TID=descriptive_7| SID=7, TID=descriptive_8| SID=7, TID=descriptive_9 | SID=7, TID=enable_0|SID=7, TID=enable_1 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_7.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_8.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Ddescriptive_9.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Denable_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Denable_1.png)|

| SID=7, TID=enable_2| SID=7, TID=enable_3| SID=7, TID=enable_4 | SID=7, TID=enable_5|SID=7, TID=prevent_0 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Denable_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Denable_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Denable_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Denable_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Dprevent_0.png)|

| SID=7, TID=prevent_1| SID=7, TID=prevent_2| SID=7, TID=prevent_3 | SID=7, TID=prevent_4|SID=7, TID=prevent_5 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Dprevent_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Dprevent_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Dprevent_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Dprevent_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D7-TID%3Dprevent_5.png)|

| SID=8, TID=cause_0| SID=8, TID=cause_1| SID=8, TID=cause_2 | SID=8, TID=cause_3|SID=8, TID=cause_4 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Dcause_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Dcause_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Dcause_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Dcause_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Dcause_4.png)|

| SID=8, TID=cause_5| SID=8, TID=counterfactual_0| SID=8, TID=counterfactual_1 | SID=8, TID=counterfactual_2|SID=8, TID=counterfactual_3 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Dcause_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Dcounterfactual_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Dcounterfactual_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Dcounterfactual_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Dcounterfactual_3.png)|

| SID=8, TID=counterfactual_4| SID=8, TID=counterfactual_5| SID=8, TID=counterfactual_6 | SID=8, TID=counterfactual_7|SID=8, TID=counterfactual_8 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Dcounterfactual_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Dcounterfactual_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Dcounterfactual_6.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Dcounterfactual_7.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Dcounterfactual_8.png)|

| SID=8, TID=descriptive_0| SID=8, TID=descriptive_1| SID=8, TID=descriptive_10 | SID=8, TID=descriptive_11|SID=8, TID=descriptive_12 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_10.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_11.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_12.png)|

| SID=8, TID=descriptive_13| SID=8, TID=descriptive_14| SID=8, TID=descriptive_15 | SID=8, TID=descriptive_16|SID=8, TID=descriptive_17 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_13.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_14.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_15.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_16.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_17.png)|

| SID=8, TID=descriptive_18| SID=8, TID=descriptive_19| SID=8, TID=descriptive_2 | SID=8, TID=descriptive_20|SID=8, TID=descriptive_21 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_18.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_19.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_20.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_21.png)|

| SID=8, TID=descriptive_22| SID=8, TID=descriptive_23| SID=8, TID=descriptive_24 | SID=8, TID=descriptive_25|SID=8, TID=descriptive_26 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_22.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_23.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_24.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_25.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_26.png)|

| SID=8, TID=descriptive_27| SID=8, TID=descriptive_28| SID=8, TID=descriptive_29 | SID=8, TID=descriptive_3|SID=8, TID=descriptive_30 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_27.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_28.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_29.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_30.png)|

| SID=8, TID=descriptive_31| SID=8, TID=descriptive_32| SID=8, TID=descriptive_33 | SID=8, TID=descriptive_34|SID=8, TID=descriptive_35 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_31.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_32.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_33.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_34.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_35.png)|

| SID=8, TID=descriptive_36| SID=8, TID=descriptive_37| SID=8, TID=descriptive_4 | SID=8, TID=descriptive_5|SID=8, TID=descriptive_6 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_36.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_37.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_6.png)|

| SID=8, TID=descriptive_7| SID=8, TID=descriptive_8| SID=8, TID=descriptive_9 | SID=8, TID=enable_0|SID=8, TID=enable_1 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_7.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_8.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Ddescriptive_9.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Denable_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Denable_1.png)|

| SID=8, TID=enable_2| SID=8, TID=enable_3| SID=8, TID=enable_4 | SID=8, TID=enable_5|SID=8, TID=prevent_0 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Denable_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Denable_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Denable_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Denable_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Dprevent_0.png)|

| SID=8, TID=prevent_1| SID=8, TID=prevent_2| SID=8, TID=prevent_3 | SID=8, TID=prevent_4|SID=8, TID=prevent_5 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Dprevent_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Dprevent_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Dprevent_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Dprevent_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D8-TID%3Dprevent_5.png)|

| SID=9, TID=cause_0| SID=9, TID=cause_1| SID=9, TID=cause_2 | SID=9, TID=cause_3|SID=9, TID=cause_4 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Dcause_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Dcause_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Dcause_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Dcause_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Dcause_4.png)|

| SID=9, TID=cause_5| SID=9, TID=counterfactual_0| SID=9, TID=counterfactual_1 | SID=9, TID=counterfactual_2|SID=9, TID=counterfactual_3 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Dcause_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Dcounterfactual_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Dcounterfactual_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Dcounterfactual_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Dcounterfactual_3.png)|

| SID=9, TID=counterfactual_4| SID=9, TID=counterfactual_5| SID=9, TID=counterfactual_6 | SID=9, TID=counterfactual_7|SID=9, TID=counterfactual_8 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Dcounterfactual_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Dcounterfactual_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Dcounterfactual_6.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Dcounterfactual_7.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Dcounterfactual_8.png)|

| SID=9, TID=descriptive_0| SID=9, TID=descriptive_1| SID=9, TID=descriptive_10 | SID=9, TID=descriptive_11|SID=9, TID=descriptive_12 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_10.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_11.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_12.png)|

| SID=9, TID=descriptive_13| SID=9, TID=descriptive_14| SID=9, TID=descriptive_15 | SID=9, TID=descriptive_16|SID=9, TID=descriptive_17 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_13.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_14.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_15.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_16.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_17.png)|

| SID=9, TID=descriptive_18| SID=9, TID=descriptive_19| SID=9, TID=descriptive_2 | SID=9, TID=descriptive_20|SID=9, TID=descriptive_21 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_18.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_19.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_20.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_21.png)|

| SID=9, TID=descriptive_22| SID=9, TID=descriptive_23| SID=9, TID=descriptive_24 | SID=9, TID=descriptive_25|SID=9, TID=descriptive_26 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_22.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_23.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_24.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_25.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_26.png)|

| SID=9, TID=descriptive_27| SID=9, TID=descriptive_28| SID=9, TID=descriptive_29 | SID=9, TID=descriptive_3|SID=9, TID=descriptive_30 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_27.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_28.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_29.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_30.png)|

| SID=9, TID=descriptive_31| SID=9, TID=descriptive_32| SID=9, TID=descriptive_33 | SID=9, TID=descriptive_34|SID=9, TID=descriptive_35 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_31.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_32.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_33.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_34.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_35.png)|

| SID=9, TID=descriptive_36| SID=9, TID=descriptive_37| SID=9, TID=descriptive_4 | SID=9, TID=descriptive_5|SID=9, TID=descriptive_6 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_36.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_37.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_6.png)|

| SID=9, TID=descriptive_7| SID=9, TID=descriptive_8| SID=9, TID=descriptive_9 | SID=9, TID=enable_0|SID=9, TID=enable_1 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_7.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_8.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Ddescriptive_9.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Denable_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Denable_1.png)|

| SID=9, TID=enable_2| SID=9, TID=enable_3| SID=9, TID=enable_4 | SID=9, TID=enable_5|SID=9, TID=prevent_0 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Denable_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Denable_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Denable_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Denable_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Dprevent_0.png)|

| SID=9, TID=prevent_1| SID=9, TID=prevent_2| SID=9, TID=prevent_3 | SID=9, TID=prevent_4|SID=9, TID=prevent_5 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Dprevent_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Dprevent_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Dprevent_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Dprevent_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D9-TID%3Dprevent_5.png)|

| SID=10, TID=cause_0| SID=10, TID=cause_1| SID=10, TID=cause_2 | SID=10, TID=cause_3|SID=10, TID=cause_4 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Dcause_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Dcause_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Dcause_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Dcause_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Dcause_4.png)|

| SID=10, TID=cause_5| SID=10, TID=counterfactual_0| SID=10, TID=counterfactual_1 | SID=10, TID=counterfactual_2|SID=10, TID=counterfactual_3 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Dcause_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Dcounterfactual_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Dcounterfactual_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Dcounterfactual_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Dcounterfactual_3.png)|

| SID=10, TID=counterfactual_4| SID=10, TID=counterfactual_5| SID=10, TID=counterfactual_6 | SID=10, TID=counterfactual_7|SID=10, TID=counterfactual_8 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Dcounterfactual_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Dcounterfactual_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Dcounterfactual_6.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Dcounterfactual_7.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Dcounterfactual_8.png)|

| SID=10, TID=descriptive_0| SID=10, TID=descriptive_1| SID=10, TID=descriptive_10 | SID=10, TID=descriptive_11|SID=10, TID=descriptive_12 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_10.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_11.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_12.png)|

| SID=10, TID=descriptive_13| SID=10, TID=descriptive_14| SID=10, TID=descriptive_15 | SID=10, TID=descriptive_16|SID=10, TID=descriptive_17 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_13.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_14.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_15.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_16.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_17.png)|

| SID=10, TID=descriptive_18| SID=10, TID=descriptive_19| SID=10, TID=descriptive_2 | SID=10, TID=descriptive_20|SID=10, TID=descriptive_21 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_18.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_19.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_20.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_21.png)|

| SID=10, TID=descriptive_22| SID=10, TID=descriptive_23| SID=10, TID=descriptive_24 | SID=10, TID=descriptive_25|SID=10, TID=descriptive_26 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_22.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_23.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_24.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_25.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_26.png)|

| SID=10, TID=descriptive_27| SID=10, TID=descriptive_28| SID=10, TID=descriptive_29 | SID=10, TID=descriptive_3|SID=10, TID=descriptive_30 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_27.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_28.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_29.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_30.png)|

| SID=10, TID=descriptive_31| SID=10, TID=descriptive_32| SID=10, TID=descriptive_33 | SID=10, TID=descriptive_34|SID=10, TID=descriptive_35 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_31.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_32.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_33.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_34.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_35.png)|

| SID=10, TID=descriptive_36| SID=10, TID=descriptive_37| SID=10, TID=descriptive_4 | SID=10, TID=descriptive_5|SID=10, TID=descriptive_6 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_36.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_37.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_6.png)|

| SID=10, TID=descriptive_7| SID=10, TID=descriptive_8| SID=10, TID=descriptive_9 | SID=10, TID=enable_0|SID=10, TID=enable_1 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_7.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_8.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Ddescriptive_9.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Denable_0.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Denable_1.png)|

| SID=10, TID=enable_2| SID=10, TID=enable_3| SID=10, TID=enable_4 | SID=10, TID=enable_5|SID=10, TID=prevent_0 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Denable_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Denable_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Denable_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Denable_5.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Dprevent_0.png)|

| SID=10, TID=prevent_1| SID=10, TID=prevent_2| SID=10, TID=prevent_3 | SID=10, TID=prevent_4|SID=10, TID=prevent_5 |
|---|---|---|---|---|
|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Dprevent_1.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Dprevent_2.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Dprevent_3.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Dprevent_4.png)|![](/dataset/outputs/stats/Dataset_v230920/SID%3D10-TID%3Dprevent_5.png)|

