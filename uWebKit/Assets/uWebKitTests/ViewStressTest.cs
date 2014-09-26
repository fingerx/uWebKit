
using UnityEngine;
using System.Collections.Generic;

public class ViewStressTest : MonoBehaviour
{
	float time = 0.0f;

	int testViews = 16;

	List<UWKWebView> views = new List<UWKWebView>();

	int urlCounter = 0;

	int viewWidth = 512;
	int viewHeight = 512;

	string[] urls = new string[] {"http://www.penny-arcade.com", "http://www.google.com", "http://www.microsoft.com", "http://www.apple.com", "https://slashdot.org", 
	"http://www.bluesnews.com", "http://unity3d.com", "http://www.github.com", "https://www.facebook.com", "https://www.twitter.com"};

    public virtual void Start()
    {
    	for (int i = 0; i < testViews; i++)
    	{
            UWKWebView view = gameObject.AddComponent("UWKWebView") as UWKWebView;
            view.URL = urls[urlCounter++];
            urlCounter %= urls.Length;
            view.MaxWidth = viewWidth;
            view.MaxHeight = viewHeight;
    		views.Add(view);
    	}

    }

    void Update()
    {

        time += Time.deltaTime;

        if (time > 3.0f)
        {
            if (views.Count != 0)
            {
                int vindex;
                UWKWebView view;

                for (int i = 0; i < 4; i++)
                {
                    string url = urls[urlCounter++];
                    urlCounter %= urls.Length;

                    vindex = (int) (Random.value * (views.Count-1));

                    views[vindex].LoadURL(url);
                }

                if (Random.value < .25f)
                {
                    vindex = (int) (Random.value * (views.Count - 1));
                    view = views[vindex];
                    views.Remove(view);
                    UnityEngine.Object.Destroy(view);
                }

                if (views.Count < 8)
                {
                    while (views.Count < 16)
                    {
                        view = gameObject.AddComponent("UWKWebView") as UWKWebView;
                        view.URL = urls[urlCounter++];
                        urlCounter %= urls.Length;
                        view.MaxWidth = viewWidth;
                        view.MaxHeight = viewHeight;
                        views.Add(view);                        
                    }

                }

            }

            time = 0.0f;

        }

    }

    void OnGUI()
    {

        int x = 0;
        int y = 0;
        foreach(UWKWebView view in views)
        {
        	Rect r = new Rect (x, y, viewWidth/4, viewHeight/4);
            GUI.DrawTexture (r, view.WebTexture);

            x += viewWidth/4;
            if (x >= 512)
            {
            	x = 0;
            	y += viewHeight/4;
            }
        }
    
    }
}