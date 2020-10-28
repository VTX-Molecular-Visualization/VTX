## Conventions de nommage

ClassName
STATIC\_CONST\_MEMBER
publicMember
_privateMember
_protectedMember
publicMethod
_protectedMethod
_privateMethod
p_parameter

## Ouvrir de la data par défault

Dans vtx_app.cpp, à la fin de la fonction start(), lancer une action Open ou OpenAPI (cf. les fonctions commentées). Pour ouvrir swur le disque, le fichier doit se trouver dans un dossier /data/ au même niveau que le dossier /src/. Pour l'API il suffit de mettre les 4 caractères du pdb.

## Good Practice UI

Tous les Widgets héritants de BaseManualWidget doivent avoir au début de leur déclaration de classe la directive VTX_MANUAL_WIDGET_DECLARATION pour leur bon fonctionnement.

## Bugs à savoir (se reporter à Asana aussi)

- Ne pas lancer de VTX\_EVENT dans le receive d'un autre event. Attention car VTX\_DEBUG|INFO|LOG lancent aussi un event pour logger dans la console, donc faut pas le faire. Pour afficher du debug dans la fonction receive, il faut utiliser VTX_CONSOLE (qui log dans la console Windows mais pas dans celle de VTX). C'est à cause du mutex qui deadlock.

## C++ Divers 

- Les classes friend référencer des classes du même namespace.