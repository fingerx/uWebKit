
using UnityEngine;

public class ViewResizeTest : MonoBehaviour
{
	public string URL;
    public Vector2 Positon;
    public bool HasFocus = true;

    public int MaxWidth = 2048;
    public int MaxHeight = 2048;

    public bool TestZoom = false;

    protected UWKWebView view;

    float time;

    bool direction = false;
    float zoom = 1.0f;



    int width;
    int height;

    public virtual void Start()
    {
    	view = gameObject.AddComponent("UWKWebView") as UWKWebView;
        view.URL = URL;
        view.MaxWidth = MaxWidth;
        view.MaxHeight = MaxHeight;
    }

    void OnGUI()
    {
        time += Time.deltaTime;

        if (time > .25)
        {
            time = 0;

            if (direction)
            {
                width += 16;
                height += 16;

                zoom += 0.05f;

                if (zoom > 3.0f)
                    zoom = 3.0f;

                if (width > MaxWidth || height > MaxHeight)
                {
                    direction = !direction;
                    width = MaxWidth;
                    height = MaxHeight;
                }

            }
            else
            {
                width -= 16;
                height -= 16;            

                zoom -= 0.05f;               

                if (zoom < 0.25f)
                    zoom = 0.25f;

                if (width < 32 || height < 32)
                {
                    width = 32;
                    height = 32;
                    direction = !direction;
                }
            }

            //Debug.Log("" + width + " x " + height + " at zoom " + zoom);

            view.SetCurrentSize(width, height);
            if (TestZoom)
                view.SetZoomFactor(zoom);

        }
    	
    	if (view != null && view.Visible() && HasFocus)
    	{
	        if (Event.current.type == EventType.Layout)
	        {
		        Vector3 mousePos = Input.mousePosition;
                mousePos.y = Screen.height - mousePos.y;        
				view.ProcessMouse(mousePos);            
	        }

	        if (Event.current.isKey)
	        {
	            view.ProcessKeyboard(Event.current);
	        }
    	}    	

        if (view != null && view.Visible())
        {
            if (Event.current.type == EventType.Repaint)
            {
                Rect r = new Rect (Positon.x, Positon.y, view.CurrentWidth, view.CurrentHeight);
                GUI.DrawTexture (r, view.WebTexture);
            }
        }
    }
}