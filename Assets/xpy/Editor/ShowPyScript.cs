using UnityEngine;
using System.Collections;
using UnityEditor;
using System.IO;

[CustomEditor(typeof(UnityEditor.DefaultAsset))]
public class LuaInspector : Editor
{
    public override void OnInspectorGUI()
    {
        string path = AssetDatabase.GetAssetPath(target);
        if (path.EndsWith(".py"))
        {
            GUI.enabled = true;
            GUI.backgroundColor = new Color(63, 63, 63);

            string ss = File.ReadAllText(path);

            GUILayout.TextArea(ss);
            //EditorGUILayout.TextArea(ss);
        }
    }
}