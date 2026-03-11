using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using TMPro;


public class Player : MonoBehaviour
{
    [SerializeField] float moveSpeed = 10f;
    [SerializeField] float padding = 1f;
    [SerializeField] int maxHealth = 20;
    [SerializeField] AudioClip hitSound;
    [SerializeField] AudioClip collectSound;
    [SerializeField] TMP_Text healthText;
    private int currentHealth;
    private float xMin, xMax;
    private AudioSource audioSource;
    private Level levelManager;

    void Start()
    {
        SetUpMoveBoundaries();
        currentHealth = maxHealth;
        audioSource = GetComponent<AudioSource>();
        levelManager = FindObjectOfType<Level>();
        UpdateHealthText();
    }

    void Update()
    {
        Move();
    }

    private void Move()
    {
        var deltaX = Input.GetAxis("Horizontal") * Time.deltaTime * moveSpeed;
        var newXPos = Mathf.Clamp(transform.position.x + deltaX, xMin, xMax);
        transform.position = new Vector2(newXPos, transform.position.y);
    }

    private void SetUpMoveBoundaries()
    {
        Camera gameCamera = Camera.main;
        xMin = gameCamera.ViewportToWorldPoint(new Vector3(0, 0, 0)).x + padding;
        xMax = gameCamera.ViewportToWorldPoint(new Vector3(1, 0, 0)).x - padding;
    }

    void OnTriggerEnter2D(Collider2D other)
    {
        if (other.CompareTag("Enemy"))
        {
            TakeDamage(1);
        }
        else if (other.CompareTag("Ball"))
        {
            CollectBall(other.gameObject);
            ScoreManager.Instance.AddScore(10);
        }
    }

    private void TakeDamage(int damage)
    {
        currentHealth -= damage;
        UpdateHealthText();

        if (hitSound != null && audioSource != null)
        {
            audioSource.PlayOneShot(hitSound);
        }

        if (currentHealth <= 0)
        {
            Die();
        }
    }


    private void UpdateHealthText()
    {
        if (healthText != null)
        {
            healthText.text = "HEALTH: " + currentHealth;
        }
    }

    private void CollectBall(GameObject ball)
    {
        Debug.Log("Ball Collected!");

        if (collectSound != null && audioSource != null)
        {
            audioSource.PlayOneShot(collectSound);
        }

        Destroy(ball);
        if (BallManager.Instance != null)
        {
            BallManager.Instance.BallCollectedOrDestroyed();
        }
    }

    private void Die()
    {
        Debug.Log("Player has died!");
        if (levelManager != null)
        {
            levelManager.LoadGameOver();
        }
        Destroy(gameObject);
    }
}
