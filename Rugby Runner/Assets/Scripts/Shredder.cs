using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Shredder : MonoBehaviour
{
    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.CompareTag("Ball")) 
        {
            
            Destroy(collision.gameObject);

            
            if (BallManager.Instance != null)
            {
                BallManager.Instance.BallCollectedOrDestroyed();
            }
        }
        else if (collision.CompareTag("Enemy")) 
        {
            
            Destroy(collision.gameObject);
        }
    }
}