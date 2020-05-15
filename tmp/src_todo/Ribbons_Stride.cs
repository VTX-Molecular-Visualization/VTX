using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class Ribbons {
	private const int HELIX = 0;
	private const int STRAND = 1;
	private const int COIL = 2;
	private const int LHANDED = -1;
	private const int RHANDED = 1;
		
	private void CalculateSecondaryStructures(List<Dictionary<string, Vector3>> residueDicts, int[] ss, int[] handedness) {
		Debug.Log("Ribbons.cs::CalculateSecondaryStructures > Beginning.");
		Vector3 c0, n1, ca1, c1, n2;
		c0 = n1 = ca1 = c1 = n2 = Vector3.zero; // you have to init them
		Dictionary<string, Vector3> res0, res1, res2;
		int nbResDicts = residueDicts.Count;
		
		float[] phi = new float[nbResDicts];
		float[] psi = new float[nbResDicts];
		bool success = false;
		for(int i=0; i<nbResDicts; i++) {
			if(i == 0 || i == nbResDicts-1) {
				phi[i] = 90f;
				psi[i] = 90f;
			} else {
				res0 = residueDicts[i-1];
				res1 = residueDicts[i];
				res2 = residueDicts[i+1];
				
				success = true;
				
				success = success && res0.TryGetValue("C", out c0);
				success = success && res1.TryGetValue("N", out n1);
				success = success && res1.TryGetValue("CA", out ca1);
				success = success && res1.TryGetValue("C", out c1);
				success = success && res2.TryGetValue("N", out n2);
				
				if(!success)
					Debug.Log("Ribbons.cs::CalculateSecondaryStructures > Failed to get all the vectors.");
		
				bool print = (i<100);
				print = false;
				
				if(print)
					Debug.Log(i.ToString() + ": " + c0.ToString() + " " + n1.ToString() + " " + ca1.ToString() + " " + c1.ToString() + " " + n2.ToString());
				
				
				phi[i] = CalculateTorsionalAngle(c0, n1, ca1, c1, print);
				psi[i] = CalculateTorsionalAngle(n1, ca1, c1, n2, print);
				
			}
		}
		
		int firstHelix = 0;
		int nconsRHelix = 0;
		int nconsLHelix = 0;
		int firstStrand = 0;
		int nconsStrand = 0;
		float extension = 0f;
		float strandExtension = 0f;
		for(int i=0; i<nbResDicts; i++) {
			// Right-handed helix
			if((WeirdDistance(phi[i], psi[i], -60f, -45f) < (30f + extension)) && (i < nbResDicts - 1)) {
				if(nconsRHelix == 0)
					firstHelix = i;
				nconsRHelix++;
			} else {
				if(3 <= nconsRHelix) {
					for(int k=firstHelix; k<i; k++) {
						ss[k] = HELIX;
						handedness[k] = RHANDED;
					}
				}
				nconsRHelix = 0;
			}
			
			// Left-handed helix
			if((WeirdDistance(phi[i], psi[i], 60f, 45f) < (30f + extension)) && (i < nbResDicts - 1)) {
				if(nconsLHelix == 0)
					firstHelix = i;
				nconsLHelix++;
			} else {
				if(3 <= nconsLHelix) {
					for(int k=firstHelix; k<i; k++) {
						ss[k] = HELIX;
						handedness[k] = LHANDED;
					}
				}
				nconsLHelix = 0;
			}
			
			// Strand
			if((WeirdDistance(phi[i], psi[i], -110f, 130f) < (30f + extension + strandExtension))
																				&& (i< nbResDicts - 1)) {
				if(nconsStrand == 0)
					firstStrand = i;
				nconsStrand++;
			} else {
				if(2 <= nconsStrand) {
					for(int k=firstStrand; k<i; k++) {
						ss[k] = STRAND;
						handedness[k] = RHANDED;
					}
				}
				nconsStrand = 0;
			}
			
			ss[i] = COIL;
			handedness[i] = RHANDED;
		}
	} // End of CalculateSecondaryStructures
	
	/// <summary>
	/// Finds helices by chain in ssHelixList (informations from the pdb file).
	/// </summary>
	/// <returns>A list of helix.</returns>
	/// <param name="chain">Chain.</param>
	private List<float[]> FindHelix (string chain){

		List<float[]> helix = Molecule.Model.MoleculeModel.ssHelixList;
		List<float[]> helixlist = new List<float[]> ();

		for (int i = 0; i < helix.Count; i++) {
			if (Molecule.Model.MoleculeModel.helixChainList [i] == chain)
				helixlist.Add (helix [i]);
		}

		// bug when list goes empty (dunno if necessary anymore)
		if(helixlist.Count != 0)
			helixlist.Add (helixlist[helixlist.Count - 1]);
		return helixlist;
	}

	/// <summary>
	/// Find strands by chain in ssStrandList (informations from the pdb file).
	/// </summary>
	/// <returns>A list of strand.</returns>
	/// <param name="chain">Chain.</param>
	private List<float[]> FindStrand (string chain){
		
		List<float[]> strand = Molecule.Model.MoleculeModel.ssStrandList;
		List<float[]> strandlist = new List<float[]> ();
		
		for (int i = 0; i < strand.Count; i++) {
			if (Molecule.Model.MoleculeModel.strandChainList [i] == chain){
				strandlist.Add (strand [i]);
			}
		}
		return strandlist;
	}

	private int ComputeNbRes(string chain){

		List<string> reschain = Molecule.Model.MoleculeModel.resChainList2;
		int nbres = 0;

		for (int i = 0; i < reschain.Count; i++) {
			if(reschain[i] == chain)
				nbres++;
		}

		return nbres;
	}

	private List<float[]> SortStrand( List<float[]> tmpstrandlist){

		for (int i = 0; i < tmpstrandlist.Count; i++) {
			for(int j = i; j < tmpstrandlist.Count; j++){
				if(tmpstrandlist[j][0] < tmpstrandlist[i][0]){
					float[] tmp = tmpstrandlist[i];
					tmpstrandlist[i] = tmpstrandlist[j];
					tmpstrandlist[j] = tmp;
				}
			}
		}

		int index = 0;

		while (index < tmpstrandlist.Count - 1)
		{
			if (tmpstrandlist[index][0] == tmpstrandlist[index + 1][0])
				tmpstrandlist.RemoveAt(index);
			else
				index++;
		}

		tmpstrandlist.Add (tmpstrandlist [tmpstrandlist.Count - 1]);
		return tmpstrandlist;
	} // End SortStrand	
}
