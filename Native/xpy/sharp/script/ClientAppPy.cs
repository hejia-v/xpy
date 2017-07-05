using UnityEngine;
using XPython;

public class ClientAppPy : MonoBehaviour
{
    void Awake()
    {
        DontDestroyOnLoad(transform.gameObject);
    }

    void Start()
    {
    }

    void OnDestroy()
    {

    }

    void FixedUpdate()
    {

    }
}
