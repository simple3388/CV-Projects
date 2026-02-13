using UnityEngine;

public class Exit : MonoBehaviour
{
    public bool IsOpen { get; private set; } = false;
    // which level this exit belongs to (set by InitializeLevel when created)
    public int levelIndex = 1;

    public Sprite closedSprite;
    public Sprite openSprite;

    private SpriteRenderer sr;

    private void Awake()
    {
        sr = GetComponent<SpriteRenderer>();
        UpdateSprite();
    }

    public void Open()
    {
        if (IsOpen) return;
        IsOpen = true;
        UpdateSprite();
    }

    private void UpdateSprite()
    {
        if (sr == null) return;
        if (IsOpen && openSprite != null) sr.sprite = openSprite;
        else if (!IsOpen && closedSprite != null) sr.sprite = closedSprite;
    }
}
