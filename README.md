# Pidgeon Engine

## About Engine
This engine is a project developed in Videogame Engine subject from Game Design and Development Degree in CITM.

Project repository: https://github.com/Landama01/PidgeonEngine

Project Forked from: https://github.com/laiapachon/ThomasEngine

## Team Members
Andreu Landa: https://github.com/Landama01

## Using Engine:

### Camera controls:
	- LSHIFT: Use controls faster
	- F: Front view
	- E: Moving camera Up
	- Q: Moving camera Down
	- W: Moving camera to Front
	- S: Moving camera Back
	- A: Moving camera Left
	- D: Moving camera Right
	- MOUSE WHEEL: Zoom In and Out
	- Alt+Left click: orbit around the camera position
	
	
### Short Cuts:
- Numbers from 1-9 open and close the tabs
- Ctrl+N: New Scene
- Ctrl+S: Save Scene
- Ctrl+D: Duplicate Object
- Ctrl+Shift+N: New GameObject Empty
- Ctrl+Shift+G: New GameObject Parent
- Alt+Shift+N: New GameObject Child
- Ctrl+Shift+F: Align with view
- Ctrl+Alt+W: Use TranslateSnap
- Ctrl+Alt+E: Use RotateSnap
- Ctrl+Alt+R: Use ScaleSnap
- Alt+Shift+F: Align with selected
- Alt+Shift+A: Toggle active state
- Alt+F: ResetViewRotation


### Important Issues found:
- Deleting duplicated game objects may cause a crash
- Deleting game objects in general, can cause a random crash
- Resource manager does not create a custom file format
- Scene Serialization Load does not render any Game Object, but "works"
- Unable to make a functional Play/Pause/Stop System
- Guizmo only works for translation of Game Objects purpose, Rotation and Scale only work using the inspector
- Asset import can fail

