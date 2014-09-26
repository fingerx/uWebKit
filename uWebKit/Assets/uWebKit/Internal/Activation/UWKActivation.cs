using UnityEngine;
using System.Collections;

#if UNITY_EDITOR
using UnityEditor;


using System;

/// <summary>
/// Internal class used for product activation
/// </summary>
public class UWKActivation : MonoBehaviour
{

	static Rect windowRect = new Rect (0, 0, 400, 300);

	void Awake ()
	{
		// ensure Core is up
		UWKCore.Init ();
	}

	void Start()
	{
		Center ();
	}


	bool validateKey (string key)
	{

		if (!key.StartsWith ("P-") && !key.StartsWith ("S-")) {
			return false;
		}
		
		if (key.Length != 21)
			return false;
		
		int count = 0;
		foreach (char c in key)
			if (c == '-')
				count++;
		
		if (count != 4)
			return false;
		
		return true;
	}


	public static void SetActivationState(int state)
	{
		activationState = state;
		activating = false;

		if (!showActivationMessage)
			return;
			
		// act1 or act2
		if (activationState == 1) 
		{
			activating = false;
			activateWindow = false;			
			EditorUtility.DisplayDialog ("uWebKit Activated", "Thank you!", "Ok");			
			EditorApplication.ExecuteMenuItem ("Edit/Play");			
		}

		else if (activationState == 2) 
		{
			activating = false;
			EditorUtility.DisplayDialog ("uWebKit Activation", "This key is invalid, please check the key and try again.\n", "Ok");
		}

		else if (activationState == 4) 
		{
			// no activations			
			activating = false;
			EditorUtility.DisplayDialog ("uWebKit Activation Failed", "Activation Count exceeded, please contact sales@uwebkit.com for more information", "Ok");
		}

		else if (activationState == 5) 
		{
			// problem
			activating = false;
			activateWindow = false;		
			EditorUtility.DisplayDialog ("uWebKit Activation", "There was an issue contacting the Activation Server.\n\nThe product is available, however you may be asked to activate again.", "Ok");			
			EditorApplication.ExecuteMenuItem ("Edit/Play");			
		}
		else if (activationState == 3)
		{
			return;
		}

	}


	void windowFunction (int windowID)
	{
		Rect titleRect = new Rect (0, 0, 400, 24);
		
		if (!activating) {

			GUILayout.BeginVertical ();
			
			Color previousColor = GUI.color;
			
			GUI.color = Color.cyan;

			GUILayout.Space (8);	
			
			GUILayout.Label ("IMPORTANT: Please ensure Build Settings are set to PC/Mac Standalone before activating");
			
			GUI.color = previousColor;
			
			GUILayout.Space (32);			
			
			GUILayout.BeginHorizontal ();
			
			GUILayout.Label ("Activation Code", GUILayout.Width (96));
			
			activationCode = GUILayout.TextField (activationCode, 64, GUILayout.Width (280)).Trim ();
			
			// we're catching p on command-p to run scene
			if (activationCode.StartsWith ("p")) {
				activationCode = "";
			}
			
			GUILayout.EndHorizontal ();
			
			GUILayout.Space (64);

			GUILayout.BeginHorizontal ();
			
			if (GUILayout.Button ("Activate", GUILayout.Height (64))) {

				if (!validateKey (activationCode)) 
				{
					EditorUtility.DisplayDialog ("uWebKit Activation", "This key is invalid, please check the key and try again.\n", "Ok");
				} 
				else 
				{
					showActivationMessage = true;
					activating = true;
					UWKPlugin.UWK_MsgActivate(activationCode);
				}
			}

			if (GUILayout.Button ("Purchase", GUILayout.Height (64))) {
				
				Application.OpenURL ("http://www.uwebkit.com/uwebkit/store");
				
			}

			if (GUILayout.Button ("Demo", GUILayout.Height (64))) {
				
				Application.OpenURL ("http://www.uwebkit.com/download");
				
			}


			GUILayout.EndHorizontal ();
			
			GUILayout.EndVertical ();			

			
		} else {
			GUILayout.Label ("Activating... Please Wait");
		}
		
		GUI.DragWindow (titleRect);
		
	}

	void OnGUI ()
	{
		if (activateWindow)
			windowRect = GUILayout.Window (-1, windowRect, windowFunction, "uWebKit Activation");
	}


	void Update ()
	{

	}

	void reset ()
	{
		activating = false;
		activateWindow = true;
		Center ();
	}
	
	// Get the center position
	public void GetCenterPos (ref Vector2 pos)
	{
		pos.x = Screen.width / 2 - windowRect.width / 2;
		pos.y = Screen.height / 2 - windowRect.height / 2;
	}
	
	// Center the browser on the screen
	public void Center ()
	{
		Vector2 v = new Vector2 ();
		
		GetCenterPos (ref v);
		
		windowRect.x = v.x;
		windowRect.y = v.y;
	}


	string activationCode = "";
	static bool activating = false;
	static bool activateWindow = true;
	static bool showActivationMessage = false;
	static int activationState = -1;

}

#endif

