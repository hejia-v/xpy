using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace UnityEngine
{
    public class Object
    {
        public static void DontDestroyOnLoad(Object target) { }
    }

    public class Component : Object
    {
        public Component()
        {

        }

        public Transform transform { get; }
        public GameObject gameObject { get; }
    }

    public class Transform : Component
    {
        GameObject _gameObject;
        public Transform()
        {
            _gameObject = new GameObject();
        }
    }

    public sealed class GameObject : Object
    {
        public GameObject()
        {

        }
    }

    public class MonoBehaviour : Component
    {
        Transform _transform;
        public MonoBehaviour()
        {
            _transform = new Transform();

        }

        public new Transform transform
        { get { return _transform; } }

        void Awake()
        {

        }

        void Start()
        {

        }

        void Update()
        {

        }

        void OnGUI()
        {

        }

        void FixedUpdate()
        {

        }

        void OnDestroy()
        {

        }
    }
}
