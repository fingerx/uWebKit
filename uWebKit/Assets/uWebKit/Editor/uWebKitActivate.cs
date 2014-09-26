/******************************************
  * uWebKit 
  * (c) 2013 THUNDERBEAST GAMES, LLC
  * sales@uwebkit.com
*******************************************/

//#define UWK_ASSETSTORE_BUILD	

using UnityEngine;
using UnityEditor;

#if !UWK_ASSETSTORE_BUILD

public class uWebKitActivate : ScriptableObject {
	
	
    [MenuItem ("uWebKit/Activate")]
    static void Activate() {
		
		if (EditorApplication.isPlaying)
		{
 			EditorApplication.isPaused = false;
        	EditorApplication.isPlaying = false;			
		}
					
		EditorApplication.OpenScene("Assets/StreamingAssets/uWebKit/Activation/uWebKitActivation.unity");
		EditorApplication.ExecuteMenuItem("Edit/Play");
		
		return;
			
    }
	        
}

#endif