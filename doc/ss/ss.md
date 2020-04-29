# Secondary structure

## Helix identification in PDB

switch (helix_type) {
  case 1: // Treat right and left handed helixes the same.
  case 6:
  	"alpha helix";
    	break;
  case 2:
  case 7:
   	"omega helix";
    	break;
  case 3:
  	"pi helix";
    	break;
  case 4:
  case 8:
   	"gamma helix";
    	break;
  case 5:
   	"3-10 helix";
    	break;
  default:
    break;
  }



If type > 5, helix is right-handed.

