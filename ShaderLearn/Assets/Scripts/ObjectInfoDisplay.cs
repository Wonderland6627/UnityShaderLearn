using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ObjectInfoDisplay : MonoBehaviour 
{
    public MeshFilter meshFilter;

    private void Start()
    {
        meshFilter = GetComponent<MeshFilter>();

        var vertices = meshFilter.mesh.vertices;
        for (int i = 0; i < vertices.Length; i++)
        {
            Debug.Log(vertices[i] + "  " + vertices[i].normalized);
        }
    }

    private void OnDrawGizmos()
    {
        /*if(meshFilter == null)
        {
            return;
        }

        var vertices = meshFilter.mesh.vertices;
        for (int i = 0; i < vertices.Length; i++)
        {
            Gizmos.DrawLine(vertices[i], vertices[i].normalized);
        }*/
    }
}
