using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class Ribbons {
	// For handling multiple chains.
	public static bool mustSplitDictList = false;
	private Vector3 flipTestV;
	private BSpline splineCenter;
	private BSpline splineSide1;
	private BSpline splineSide2;
	private int uspacing;
	private int vIndex;

	public List<int> handedlist = new List<int> ();
	public List<int> sslist = new List<int> ();
	private bool createss = false; // check if ss informations are present in the pdb file
	public static string ribbontag = "";
	private static string currentchain = "";

	private const int HELIX = 0;
	private const int STRAND = 1;
	private const int COIL = 2;
	private const int LHANDED = -1;
	private const int RHANDED = 1;
	
	private static int RIBBON_DETAIL = 4;           // Ribbon detail: from 1 (lowest) to 4 (highest)
	private static int RENDER_MODE = 1;             // 0 = lines, 1 = flat ribbons
	
	public static ColorObject HELIX_COLOR = new ColorObject(new Color32(255,0,0,255));	// red
	public static ColorObject STRAND_COLOR = new ColorObject(new Color32(0,0,255,255));	// blue
	public static ColorObject COIL_COLOR = new ColorObject(new Color32(255,255,255,255));	// light grey

	public static ColorObject ChainColorA = new ColorObject(new Color32( 10,  3,200,50));
	public static ColorObject ChainColorB = new ColorObject(new Color32( 10,170, 34,50));
	public static ColorObject ChainColorC = new ColorObject(new Color32(  0,150,150,50));
	public static ColorObject ChainColorD = new ColorObject(new Color32(  0,180,  0,50));
	public static ColorObject ChainColorE = new ColorObject(new Color32(  0,170,200,50));

	public static float HELIX_DIAM = 1.8f;
	public static float THICKNESS = 0.3f;
	public static float ARROW_WIDTH = 1.8f;
	
	public static float[] ribbonWidth = {1.5f, 1.7f, 0.3f}; // Ribbon widths for helix, strand and coil, respectively

    public void CreateRibbons()
    {

        List<Dictionary<string, Vector3>> residueDicts = Molecule.Model.MoleculeModel.residueDictionaries;
        CreateRibbons(residueDicts, sslist, handedlist);
    }

    private void CreateRibbons(List<Dictionary<string, Vector3>> residueDicts, List<int> sslist, List<int> handedlist)
    {
        //List<Dictionary<string, Vector3>> residueDicts = Molecule.Model.MoleculeModel.residueDictionaries;

        // For line ribbons
        List<Vector3> vertices0 = new List<Vector3>();
        List<Vector3> vertices1 = new List<Vector3>();
        List<Vector3> vertices2 = new List<Vector3>();

        // For flat ribbons
        List<Vector3> vertices = new List<Vector3>();
        List<Color32> colors = new List<Color32>();
        List<Vector3> normals = new List<Vector3>();

        switch (RIBBON_DETAIL)
        {
            case 1:
                uspacing = 10;
                break;
            case 2:
                uspacing = 5;
                break;
            case 3:
                uspacing = 2;
                break;
            default:
                uspacing = 1;
                break;
        }

        flipTestV = new Vector3();
        splineSide1 = new BSpline(false);
        splineCenter = new BSpline(false);
        splineSide2 = new BSpline(false);

        int[] ss = new int[residueDicts.Count];
        int[] handedness = new int[residueDicts.Count];
        List<int> ssglic = new List<int>(); // Specific to Glic for now (split differents domains)
        int nbRes = residueDicts.Count;
        int colorOffset = nbRes - 1;


        ss = sslist.ToArray();
        handedness = handedlist.ToArray();


        InitCol();  //Initialization Dictionary of colors

        List<int> triangles = new List<int>();
        vIndex = 0;
        bool isArrow;
        for (int i = 0; i < nbRes; i++)
        {

            ConstructControlPoints(residueDicts, i, ss[i], handedness[i]);
            int colorIndex = (i + colorOffset) % nbRes;
            //int arrowIndex = (i+arrowOffset) % nbRes;
            isArrow = (ss[colorIndex] == STRAND && ((colorIndex + 1 == nbRes) || (ss[colorIndex + 1] != STRAND)));


            if (isArrow && ARROW_WIDTH > 0f)
                GenerateArrowRibbon(vertices, normals, triangles);
            else
                GenerateFlatRibbon(vertices, normals, triangles);
            Color32 color;

            if (UI.UIData.ssColChain)
            {
                color = GetColorChain(currentchain);
                if (i > 185 && UI.UIData.ssDivCol)
                {
                    color.g += 100;
                    color.b += 100;
                }
            }
            else
            {

                switch (ss[colorIndex])
                {
                    case HELIX:
                        color = HELIX_COLOR.color;
                        break;
                    case STRAND:
                        color = STRAND_COLOR.color;
                        break;
                    default:
                        color = COIL_COLOR.color;
                        break;
                }
            }
            for (int j = 0; j < 320; j++)
                colors.Add(color);

            ssglic.Add(ss[i]);
        }

        PostProcessing.GenerateMeshes(vertices, normals, triangles, colors, ss);


    } // End of CreateRibbons	







    private void ConstructControlPoints(List<Dictionary<string, Vector3>> residueDicts, int res, int ss, int handedness)
    {
        //Debug.Log("Ribbons.cs::ConstructControlPoints > Beginning.");
        Vector3 ca0, ox0, ca1;
        Vector3 p0, p1, p2, p3;

        p1 = p2 = p3 = Vector3.zero;

        Dictionary<string, Vector3> res0, res1;
        res0 = res1 = null;

        if (res == 0)
        {
            // The control points 2 and 3 are created
            flipTestV = Vector3.zero;

            res0 = residueDicts[res];
            res1 = residueDicts[res + 1];

            bool success = true;
            success = success && res0.TryGetValue("CA", out ca0);
            success = success && res0.TryGetValue("O", out ox0);
            success = success && res1.TryGetValue("CA", out ca1);

            if (!success)
                Debug.Log("Ribbons.cs::ConstructControlPoints > Failed to get all the vectors.");

            AddControlPoints(ca0, ox0, ca1, ss, handedness);
            splineSide1.CopyCPoints(3, 2);
            splineCenter.CopyCPoints(3, 2);
            splineSide2.CopyCPoints(3, 2);

            res0 = residueDicts[res + 1];
            res1 = residueDicts[res + 2];
            success = true;
            success = success && res0.TryGetValue("CA", out ca0);
            success = success && res0.TryGetValue("O", out ox0);
            success = success && res1.TryGetValue("CA", out ca1);
            if (!success)
                Debug.Log("Ribbons.cs::ConstructControlPoints > Failed to get all the vectors.");
            AddControlPoints(ca0, ox0, ca1, ss, handedness);

            // We still need the two first control points
            // Moving backwards along the cp_center[2] - cp_center[3] direction
            splineCenter.GetCPoint(2, out p2);
            splineCenter.GetCPoint(3, out p3);

            p1 = LinearComb(2f, p2, -1f, p3);
            splineCenter.SetCPoint(1, p1);
            splineSide1.SetCPoint(1, LinearComb(1f, p1, ribbonWidth[ss], flipTestV));
            splineSide2.SetCPoint(1, LinearComb(1f, p1, -ribbonWidth[ss], flipTestV));

            p0 = LinearComb(2f, p1, -1f, p2);


            splineCenter.SetCPoint(0, p0);
            splineSide1.SetCPoint(0, LinearComb(1f, p0, ribbonWidth[ss], flipTestV));
            splineSide2.SetCPoint(0, LinearComb(1f, p0, -ribbonWidth[ss], flipTestV));
        }
        else
        {
            ShiftControlPoints();
            if ((residueDicts.Count - 1 == res) || (residueDicts.Count - 2 == res))
            {
                // Moving forward along the cp_center[1] - cp_center[2] direction
                splineCenter.GetCPoint(1, out p1);
                splineCenter.GetCPoint(2, out p2);

                p3 = LinearComb(2f, p2, -1f, p1);
                splineCenter.SetCPoint(3, p3);
                splineSide1.SetCPoint(3, LinearComb(1f, p3, ribbonWidth[ss], flipTestV));
                splineSide2.SetCPoint(3, LinearComb(1f, p3, -ribbonWidth[ss], flipTestV));
            }
            else
            {
                res0 = residueDicts[res + 1];
                res1 = residueDicts[res + 2];
                bool success = true;
                success = success && res0.TryGetValue("CA", out ca0);
                success = success && res0.TryGetValue("O", out ox0);
                success = success && res1.TryGetValue("CA", out ca1);
                if (!success)
                    Debug.Log("Ribbons.cs::ConstructControlPoints > Failed to get all the vectors.");
                AddControlPoints(ca0, ox0, ca1, ss, handedness);
            }
        }
        splineSide1.UpdateMatrix3();
        splineCenter.UpdateMatrix3();
        splineSide2.UpdateMatrix3();
    } // End of ConstructControlPoints






    // Adds a new control point to the arrays CPCenter, CPRight and CPLeft
    private void AddControlPoints(Vector3 ca0, Vector3 ox0, Vector3 ca1, int ss, int handedness) {
		
		Vector3 A, B, C, D, p0, cpt0, cpt1, cpt2;
		
		A = ca1 - ca0;
		B = ox0 - ca0;
		
		// Vector normal to the peptide plane (pointing outside in the case of the
		// alpha helix).
		C = Vector3.Cross(A,B);
		
		// Vector contained in the peptide plane (perpendicular to its direction).
		D = Vector3.Cross(C,A);
		
		C.Normalize();
		D.Normalize();
		
		// Flipping test (to avoid self crossing in the strands).
		if( (ss != HELIX) && (90f < Vector3.Angle(flipTestV, D) ) )
			D = D * -1f; // flip detected, the plane vector is inverted
		
		// The central control point is constructed
		cpt0 = LinearComb(0.5f, ca0, 0.5f, ca1);
		splineCenter.SetCPoint(3, cpt0);
		
		if(ss == HELIX) {
			// When residue i is contained in a helix, the control point is moved away
			// from the helix axis, along the C direction.
			p0 = Vector3.zero;
			splineCenter.GetCPoint(3, out p0);
			cpt0 = LinearComb(1f, p0, handedness*HELIX_DIAM, C);
			splineCenter.SetCPoint(3, cpt0);
		}
		
		// The control points for the side ribbons are constructed.
		cpt1 = LinearComb(1f, cpt0, ribbonWidth[ss], D);
		splineSide1.SetCPoint(3, cpt1);
		
		cpt2 = LinearComb(1f, cpt0, - ribbonWidth[ss], D);
		splineSide2.SetCPoint(3, cpt2);


		// Saving the plane vector (for the flipping test in the next call)
		flipTestV = D;		
	} // End of AddControlPoints
	
	
	private void ShiftControlPoints() {
		splineSide1.ShiftBSplineCPoints();
		splineCenter.ShiftBSplineCPoints();
		splineSide2.ShiftBSplineCPoints();
	}
	
	
	
	



	













	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///
	private void GenerateSpline(int n, List<Vector3> vertices)
	{
		int ui;
		float u;
		Vector3 v1; // original Processing code includes a v0 that is never used

		v1 = Vector3.zero;

		if (n == 0)
			splineSide1.Feval(0f, out v1);
		else
			if (n == 1)
			splineCenter.Feval(0f, out v1);
		else
			splineSide2.Feval(0f, out v1);
		vertices.Add(new Vector3(v1.x, v1.y, v1.z));

		for (ui = 1; ui <= 10; ui++)
		{
			if (ui % uspacing == 0)
			{
				u = 0.1f * ui;

				if (n == 0)
					splineSide1.Feval(u, out v1);
				else
					if (n == 1)
					splineCenter.Feval(u, out v1);
				else
					splineSide2.Feval(u, out v1);

				vertices.Add(new Vector3(v1.x, v1.y, v1.z));
			}
		}
	} // End of GenerateSpline

	private void GenerateArrowRibbon(List<Vector3> vertices, List<Vector3> normals, List<int> triangles)
	{
		Vector3 CentPoint0, CentPoint1, Sid1Point0, Sid1Point1, Sid2Point0, Sid2Point1,
				Transversal, Tangent, Normal0, Normal1;
		Vector3 CP0, CP1, S1P0, S1P1, S2P0, S2P1, Norm0, Norm1;
		Vector3 leftVect0, rightVect0, leftVect1, rightVect1;
		Vector3 Forward, LeftDiag0, RightDiag0, LeftDiag1, RightDiag1;
		Forward = LeftDiag0 = RightDiag0 = LeftDiag1 = RightDiag1 = Vector3.zero;
		leftVect0 = leftVect1 = rightVect0 = rightVect1 = Vector3.zero;
		int ui;
		float u = 0f; // needs to be assigned
		CentPoint0 = CentPoint1 = Sid1Point0 = Sid1Point1 = Sid2Point0 = Sid2Point1 = Transversal = Tangent = Normal0 = Normal1 = Vector3.zero;
		CP0 = CP1 = S1P0 = S1P1 = S2P0 = S2P1 = Norm0 = Norm1 = Vector3.zero;
		// The initial geometry is generated
		splineSide1.Feval(0f, out Sid1Point1);
		splineCenter.Feval(0f, out CentPoint1);
		splineSide2.Feval(0f, out Sid2Point1);

		// The tangents at the three previous points are the same
		splineSide2.Deval(0f, out Tangent);

		// Vector transversal to the ribbon
		Transversal = Sid1Point1 - Sid2Point1;
		Normal1 = Vector3.Cross(Transversal, Tangent);
		Normal1.Normalize();
		float extraWidthFactor = ARROW_WIDTH;
		float pointOneAdjustment = 0.1f * ARROW_WIDTH;
		float pointOneWidthFactor;

		Vector3 leftNormal0, leftNormal1, rightNormal0, rightNormal1;
		for (ui = 1; ui <= 10; ui++)
		{
			if (ui % uspacing == 0)
			{
				u = 0.1f * (float)ui;
				pointOneWidthFactor = extraWidthFactor - pointOneAdjustment;

				// The geometry of the previous iteration is saved
				Sid1Point0 = Sid1Point1;
				CentPoint0 = CentPoint1;
				Sid2Point0 = Sid2Point1;
				Normal0 = Normal1;

				// The new geometry is generated
				splineSide1.Feval(u, out Sid1Point1);
				splineCenter.Feval(u, out CentPoint1);
				splineSide2.Feval(u, out Sid2Point1);

				// The tangents at the three previous points are the same
				splineSide2.Deval(u, out Tangent);
				// Vector transversal to the ribbon
				Transversal = Sid1Point1 - Sid2Point1;
				Normal1 = Vector3.Cross(Transversal, Tangent);
				Normal1.Normalize();


				S1P0 = new Vector3(-Sid1Point0.x, Sid1Point0.y, Sid1Point0.z);
				S1P1 = new Vector3(-Sid1Point1.x, Sid1Point1.y, Sid1Point1.z);

				S2P0 = new Vector3(-Sid2Point0.x, Sid2Point0.y, Sid2Point0.z);
				S2P1 = new Vector3(-Sid2Point1.x, Sid2Point1.y, Sid2Point1.z);

				CP0 = new Vector3(-CentPoint0.x, CentPoint0.y, CentPoint0.z);
				CP1 = new Vector3(-CentPoint1.x, CentPoint1.y, CentPoint1.z);

				Norm0 = new Vector3(-Normal0.x, Normal0.y, Normal0.z);
				Norm1 = new Vector3(-Normal1.x, Normal1.y, Normal1.z);

				leftVect0 = S1P0 - CP0;
				leftVect1 = S1P1 - CP1;

				rightVect0 = S2P0 - CP0;
				rightVect1 = S2P1 - CP1;

				leftNormal0 = leftVect0.normalized;
				leftNormal1 = leftVect1.normalized;

				rightNormal0 = rightVect0.normalized;
				rightNormal1 = rightVect1.normalized;

				/*
				Forward = (CP1 - CP0).normalized;
				LeftDiag = (Forward + leftNormal0).normalized;
				RightDiag = (Forward + rightNormal0).normalized;
				*/

				Forward = CP1 - CP0;
				LeftDiag0 = (Forward.magnitude * Forward + (leftVect0.magnitude + ARROW_WIDTH) * leftVect0).normalized;
				LeftDiag1 = (Forward.magnitude * Forward + (leftVect1.magnitude + ARROW_WIDTH) * leftVect1).normalized;
				RightDiag0 = (Forward.magnitude * Forward + (rightVect0.magnitude + ARROW_WIDTH) * rightVect0).normalized;
				RightDiag1 = (Forward.magnitude * Forward + (rightVect1.magnitude + ARROW_WIDTH) * rightVect1).normalized;


				// The (Sid1Point0, Sid1Point1, CentPoint1) triangle is added.
				vertices.Add(S1P0 + extraWidthFactor * leftNormal0);
				normals.Add(Norm0);

				vertices.Add(S1P1 + pointOneWidthFactor * leftNormal1);
				normals.Add(Norm1);

				vertices.Add(CP1);
				normals.Add(Norm1);

				triangles.Add(vIndex);
				triangles.Add(vIndex + 1);
				triangles.Add(vIndex + 2);

				// and duplicated above
				vertices.Add(S1P0 + THICKNESS * Norm0 + extraWidthFactor * leftNormal0);
				normals.Add(-Norm0);

				vertices.Add(S1P1 + THICKNESS * Norm1 + pointOneWidthFactor * leftNormal1);
				normals.Add(-Norm1);

				vertices.Add(CP1 + THICKNESS * Norm1);
				normals.Add(-Norm1);

				triangles.Add(vIndex + 3);
				triangles.Add(vIndex + 4);
				triangles.Add(vIndex + 5);

				// The (Sid1Point0, CentPoint1, CentPoint0) triangle is added.
				vertices.Add(S1P0 + extraWidthFactor * leftNormal0);
				normals.Add(Norm0);

				vertices.Add(CP1);
				normals.Add(Norm1);

				vertices.Add(CP0);
				normals.Add(Norm0);

				triangles.Add(vIndex + 6);
				triangles.Add(vIndex + 7);
				triangles.Add(vIndex + 8);

				// and duplicated above
				vertices.Add(S1P0 + THICKNESS * Norm0 + extraWidthFactor * leftNormal0);
				normals.Add(-Norm0);

				vertices.Add(CP1 + THICKNESS * Norm1);
				normals.Add(-Norm1);

				vertices.Add(CP0 + THICKNESS * Norm0);
				normals.Add(-Norm0);

				triangles.Add(vIndex + 9);
				triangles.Add(vIndex + 10);
				triangles.Add(vIndex + 11);

				// (Sid2Point0, Sid2Point1, CentPoint1) triangle is added.
				vertices.Add(S2P0 + extraWidthFactor * rightNormal0);
				normals.Add(Norm0);

				vertices.Add(S2P1 + pointOneWidthFactor * rightNormal1);
				normals.Add(Norm1);

				vertices.Add(CP1);
				normals.Add(Norm1);

				triangles.Add(vIndex + 12);
				triangles.Add(vIndex + 13);
				triangles.Add(vIndex + 14);

				// and duplicated above
				vertices.Add(S2P0 + THICKNESS * Norm0 + extraWidthFactor * rightNormal0);
				normals.Add(-Norm0);

				vertices.Add(S2P1 + THICKNESS * Norm1 + pointOneWidthFactor * rightNormal1);
				normals.Add(-Norm1);

				vertices.Add(CP1 + THICKNESS * Norm1);
				normals.Add(-Norm1);

				triangles.Add(vIndex + 15);
				triangles.Add(vIndex + 16);
				triangles.Add(vIndex + 17);

				// (Sid2Point0, CentPoint1, CentPoint0) triangle is added.
				vertices.Add(S2P0 + extraWidthFactor * rightNormal0);
				normals.Add(Norm0);

				vertices.Add(CP1);
				normals.Add(Norm1);

				vertices.Add(CP0);
				normals.Add(Norm0);

				triangles.Add(vIndex + 18);
				triangles.Add(vIndex + 19);
				triangles.Add(vIndex + 20);

				// and duplicated above

				vertices.Add(S2P0 + THICKNESS * Norm0 + extraWidthFactor * rightNormal0);
				normals.Add(-Norm0);

				vertices.Add(CP1 + THICKNESS * Norm1);
				normals.Add(-Norm1);

				vertices.Add(CP0 + THICKNESS * Norm0);
				normals.Add(-Norm0);

				triangles.Add(vIndex + 21);
				triangles.Add(vIndex + 22);
				triangles.Add(vIndex + 23);

				// Duplicating the side vertices and giving them the proper normals
				// for the sides of the thick ribbons
				vertices.Add(S1P0 + extraWidthFactor * leftNormal0);
				normals.Add(LeftDiag0);

				vertices.Add(S1P1 + pointOneWidthFactor * leftNormal1);
				normals.Add(LeftDiag1);

				vertices.Add(S2P0 + extraWidthFactor * rightNormal0);
				normals.Add(RightDiag0);

				vertices.Add(S2P1 + pointOneWidthFactor * rightNormal1);
				normals.Add(RightDiag1);

				// and duplicating them again, this time raising them as well
				vertices.Add(S1P0 + THICKNESS * Norm0 + extraWidthFactor * leftNormal0);
				normals.Add(LeftDiag0);

				vertices.Add(S1P1 + THICKNESS * Norm1 + pointOneWidthFactor * leftNormal1);
				normals.Add(LeftDiag1);

				vertices.Add(S2P0 + THICKNESS * Norm0 + extraWidthFactor * rightNormal0);
				normals.Add(RightDiag0);

				vertices.Add(S2P1 + THICKNESS * Norm1 + pointOneWidthFactor * rightNormal1);
				normals.Add(RightDiag1);

				// Side triangles
				triangles.Add(vIndex + 24);
				triangles.Add(vIndex + 25);
				triangles.Add(vIndex + 28);

				triangles.Add(vIndex + 25);
				triangles.Add(vIndex + 28);
				triangles.Add(vIndex + 29);

				triangles.Add(vIndex + 26);
				triangles.Add(vIndex + 27);
				triangles.Add(vIndex + 30);

				triangles.Add(vIndex + 27);
				triangles.Add(vIndex + 30);
				triangles.Add(vIndex + 31);

				// Backside triangles for the arrow
				/*
				triangles.Add(vIndex+24);
				triangles.Add(vIndex+8);
				triangles.Add(vIndex+28);
				
				triangles.Add(vIndex+8);
				triangles.Add(vIndex+11);
				triangles.Add(vIndex+28);
				
				triangles.Add(vIndex+8);
				triangles.Add(vIndex+11);
				triangles.Add(vIndex+30);
				
				triangles.Add(vIndex+8);
				triangles.Add(vIndex+26);
				triangles.Add(vIndex+30);
				*/
				vIndex += 32;
				extraWidthFactor = extraWidthFactor - pointOneAdjustment;
			}
		}
	} // End of GenerateArrowRibbon

	private void GenerateFlatRibbon(List<Vector3> vertices, List<Vector3> normals, List<int> triangles)
	{
		Vector3 CentPoint0, CentPoint1, Sid1Point0, Sid1Point1, Sid2Point0, Sid2Point1,
				Transversal, Tangent, Normal0, Normal1;
		Vector3 CP0, CP1, S1P0, S1P1, S2P0, S2P1, Norm0, Norm1;
		int ui;
		float u = 0f; // needs to be assigned
		CentPoint0 = CentPoint1 = Sid1Point0 = Sid1Point1 = Sid2Point0 = Sid2Point1 = Transversal = Tangent = Normal0 = Normal1 = Vector3.zero;
		CP0 = CP1 = S1P0 = S1P1 = S2P0 = S2P1 = Norm0 = Norm1 = Vector3.zero;
		// The initial geometry is generated
		splineSide1.Feval(0f, out Sid1Point1);
		splineCenter.Feval(0f, out CentPoint1);
		splineSide2.Feval(0f, out Sid2Point1);

		// The tangents at the three previous points are the same
		splineSide2.Deval(0f, out Tangent);

		// Vector transversal to the ribbon
		Transversal = Sid1Point1 - Sid2Point1;
		Normal1 = Vector3.Cross(Transversal, Tangent);
		Normal1.Normalize();

		Vector3 leftNormal0, leftNormal1, rightNormal0, rightNormal1;
		for (ui = 1; ui <= 10; ui++)
		{
			if (ui % uspacing == 0)
			{
				u = 0.1f * (float)ui;

				// The geometry of the previous iteration is saved
				Sid1Point0 = Sid1Point1;
				CentPoint0 = CentPoint1;
				Sid2Point0 = Sid2Point1;
				Normal0 = Normal1;

				// The new geometry is generated
				splineSide1.Feval(u, out Sid1Point1);
				splineCenter.Feval(u, out CentPoint1);
				splineSide2.Feval(u, out Sid2Point1);

				// The tangents at the three previous points are the same
				splineSide2.Deval(u, out Tangent);
				// Vector transversal to the ribbon
				Transversal = Sid1Point1 - Sid2Point1;
				Normal1 = Vector3.Cross(Transversal, Tangent);
				Normal1.Normalize();

				// The x coordinates must be flipped, but the original vectors can't be
				// modified because they will be reused in the next iteration.
				S1P0 = new Vector3(-Sid1Point0.x, Sid1Point0.y, Sid1Point0.z);
				S1P1 = new Vector3(-Sid1Point1.x, Sid1Point1.y, Sid1Point1.z);

				S2P0 = new Vector3(-Sid2Point0.x, Sid2Point0.y, Sid2Point0.z);
				S2P1 = new Vector3(-Sid2Point1.x, Sid2Point1.y, Sid2Point1.z);

				CP0 = new Vector3(-CentPoint0.x, CentPoint0.y, CentPoint0.z);
				CP1 = new Vector3(-CentPoint1.x, CentPoint1.y, CentPoint1.z);

				Norm0 = new Vector3(-Normal0.x, Normal0.y, Normal0.z);
				Norm1 = new Vector3(-Normal1.x, Normal1.y, Normal1.z);

				// Left and right may be reversed, but either way,
				// these normals point outwards from the ribbons, horizontally.
				leftNormal0 = (S1P0 - CP0).normalized;
				leftNormal1 = (S1P1 - CP1).normalized;

				rightNormal0 = (S2P0 - CP0).normalized;
				rightNormal1 = (S2P1 - CP1).normalized;

				// The (Sid1Point0, Sid1Point1, CentPoint1) triangle is added.
				vertices.Add(S1P0);
				normals.Add(Norm0);

				vertices.Add(S1P1);
				normals.Add(Norm1);

				vertices.Add(CP1);
				normals.Add(Norm1);

				triangles.Add(vIndex);
				triangles.Add(vIndex + 1);
				triangles.Add(vIndex + 2);

				// and duplicated above
				vertices.Add(S1P0 + THICKNESS * Norm0);
				normals.Add(-Norm0);

				vertices.Add(S1P1 + THICKNESS * Norm1);
				normals.Add(-Norm1);

				vertices.Add(CP1 + THICKNESS * Norm1);
				normals.Add(-Norm1);

				triangles.Add(vIndex + 3);
				triangles.Add(vIndex + 4);
				triangles.Add(vIndex + 5);

				// The (Sid1Point0, CentPoint1, CentPoint0) triangle is added.
				vertices.Add(S1P0);
				normals.Add(Norm0);

				vertices.Add(CP1);
				normals.Add(Norm1);

				vertices.Add(CP0);
				normals.Add(Norm0);

				triangles.Add(vIndex + 6);
				triangles.Add(vIndex + 7);
				triangles.Add(vIndex + 8);

				// and duplicated above
				vertices.Add(S1P0 + THICKNESS * Norm0);
				normals.Add(-Norm0);

				vertices.Add(CP1 + THICKNESS * Norm1);
				normals.Add(-Norm1);

				vertices.Add(CP0 + THICKNESS * Norm0);
				normals.Add(-Norm0);

				triangles.Add(vIndex + 9);
				triangles.Add(vIndex + 10);
				triangles.Add(vIndex + 11);

				// (Sid2Point0, Sid2Point1, CentPoint1) triangle is added.
				vertices.Add(S2P0);
				normals.Add(Norm0);

				vertices.Add(S2P1);
				normals.Add(Norm1);

				vertices.Add(CP1);
				normals.Add(Norm1);

				triangles.Add(vIndex + 12);
				triangles.Add(vIndex + 13);
				triangles.Add(vIndex + 14);

				// and duplicated above
				vertices.Add(S2P0 + THICKNESS * Norm0);
				normals.Add(-Norm0);

				vertices.Add(S2P1 + THICKNESS * Norm1);
				normals.Add(-Norm1);

				vertices.Add(CP1 + THICKNESS * Norm1);
				normals.Add(-Norm1);

				triangles.Add(vIndex + 15);
				triangles.Add(vIndex + 16);
				triangles.Add(vIndex + 17);

				// (Sid2Point0, CentPoint1, CentPoint0) triangle is added.
				vertices.Add(S2P0);
				normals.Add(Norm0);

				vertices.Add(CP1);
				normals.Add(Norm1);

				vertices.Add(CP0);
				normals.Add(Norm0);

				triangles.Add(vIndex + 18);
				triangles.Add(vIndex + 19);
				triangles.Add(vIndex + 20);

				// and duplicated above
				vertices.Add(S2P0 + THICKNESS * Norm0);
				normals.Add(-Norm0);

				vertices.Add(CP1 + THICKNESS * Norm1);
				normals.Add(-Norm1);

				vertices.Add(CP0 + THICKNESS * Norm0);
				normals.Add(-Norm0);

				triangles.Add(vIndex + 21);
				triangles.Add(vIndex + 22);
				triangles.Add(vIndex + 23);

				// Duplicating the side vertices and giving them the proper normals
				// for the sides of the thick ribbons
				vertices.Add(S1P0);
				normals.Add(leftNormal0);

				vertices.Add(S1P1);
				normals.Add(leftNormal1);

				vertices.Add(S2P0);
				normals.Add(rightNormal0);

				vertices.Add(S2P1);
				normals.Add(rightNormal1);

				// and duplicating them again, this time raising them as well
				vertices.Add(S1P0 + THICKNESS * Norm0);
				normals.Add(leftNormal0);

				vertices.Add(S1P1 + THICKNESS * Norm1);
				normals.Add(leftNormal1);

				vertices.Add(S2P0 + THICKNESS * Norm0);
				normals.Add(rightNormal0);

				vertices.Add(S2P1 + THICKNESS * Norm1);
				normals.Add(rightNormal1);

				// Side triangles
				triangles.Add(vIndex + 24);
				triangles.Add(vIndex + 25);
				triangles.Add(vIndex + 28);

				triangles.Add(vIndex + 25);
				triangles.Add(vIndex + 28);
				triangles.Add(vIndex + 29);

				triangles.Add(vIndex + 26);
				triangles.Add(vIndex + 27);
				triangles.Add(vIndex + 30);

				triangles.Add(vIndex + 27);
				triangles.Add(vIndex + 30);
				triangles.Add(vIndex + 31);

				vIndex += 32;
			}
		}
	} // End of GenerateFlatRibbons
}

