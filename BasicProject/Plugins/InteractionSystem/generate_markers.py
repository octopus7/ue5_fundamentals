"""
Generate interaction marker images (Dot and Ring) for the InteractionSystem plugin
생성할 이미지: 원형 점(Dot), 고리(Ring)
"""

from PIL import Image, ImageDraw
import math
import os

# Output directory
output_dir = r"d:\github\ue5_fundamentals\BasicProject\Plugins\InteractionSystem\Content\Textures"
os.makedirs(output_dir, exist_ok=True)

def generate_dot(size=64, radius_ratio=0.4):
    """Generate a filled circle (dot) with smooth edges"""
    img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
    center = size // 2
    radius = size * radius_ratio
    
    for y in range(size):
        for x in range(size):
            # Calculate distance from center
            dist = math.sqrt((x - center + 0.5) ** 2 + (y - center + 0.5) ** 2)
            
            # Smooth edge using anti-aliasing
            if dist < radius - 1:
                alpha = 255
            elif dist < radius + 1:
                # Smooth transition at edge
                alpha = int(255 * (1 - (dist - radius + 1) / 2))
                alpha = max(0, min(255, alpha))
            else:
                alpha = 0
            
            img.putpixel((x, y), (255, 255, 255, alpha))
    
    return img

def generate_ring(size=64, outer_ratio=0.45, inner_ratio=0.35):
    """Generate a ring (hollow circle) with smooth edges"""
    img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
    center = size // 2
    outer_radius = size * outer_ratio
    inner_radius = size * inner_ratio
    
    for y in range(size):
        for x in range(size):
            # Calculate distance from center
            dist = math.sqrt((x - center + 0.5) ** 2 + (y - center + 0.5) ** 2)
            
            # Calculate alpha based on ring shape
            alpha = 0
            
            # Outer edge (fade out)
            if dist >= outer_radius - 1 and dist <= outer_radius + 1:
                if dist <= outer_radius:
                    alpha = int(255 * (1 - (outer_radius - dist)))
                else:
                    alpha = int(255 * (1 - (dist - outer_radius)))
            # Inner edge (fade in)
            elif dist >= inner_radius - 1 and dist <= inner_radius + 1:
                if dist >= inner_radius:
                    alpha = int(255 * (1 - (dist - inner_radius)))
                else:
                    alpha = int(255 * (1 - (inner_radius - dist)))
            # Solid ring area
            elif dist > inner_radius and dist < outer_radius:
                alpha = 255
            
            alpha = max(0, min(255, alpha))
            img.putpixel((x, y), (255, 255, 255, alpha))
    
    return img

# Generate images
print("Generating interaction marker images...")

# Dot (small filled circle)
dot_img = generate_dot(size=64, radius_ratio=0.35)
dot_path = os.path.join(output_dir, "T_InteractionDot.png")
dot_img.save(dot_path)
print(f"Created: {dot_path}")

# Ring (hollow circle)
ring_img = generate_ring(size=64, outer_ratio=0.48, inner_ratio=0.38)
ring_path = os.path.join(output_dir, "T_InteractionRing.png")
ring_img.save(ring_path)
print(f"Created: {ring_path}")

print("\nDone! Import these textures into Unreal Engine.")
print("Texture settings recommendation:")
print("  - Compression: UserInterface2D (RGBA)")
print("  - sRGB: Checked")
