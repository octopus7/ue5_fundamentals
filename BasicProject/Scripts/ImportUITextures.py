"""
Import Textures for UI - Unreal Engine 5 Python Script
UI용 텍스처 임포트 스크립트

Usage / 사용법:
1. UE5 에디터 → Tools → Execute Python Script
2. 이 스크립트 선택하여 실행

Note: 아래 FILES_TO_IMPORT 리스트에서 임포트할 파일들을 수정하세요.
"""

import unreal

# ============================================================================
# 설정 영역 - 여기서 임포트할 파일들을 지정하세요
# Configuration - Specify files to import here
# ============================================================================

# 임포트할 파일 목록 (절대 경로)
# List of files to import (absolute paths)
FILES_TO_IMPORT = [
    r"D:\github\ue5_fundamentals\BasicProject\Plugins\InteractionSystem\Content\Textures\T_InteractionDot.png",
    r"D:\github\ue5_fundamentals\BasicProject\Plugins\InteractionSystem\Content\Textures\T_InteractionRing.png",
]

# 임포트 대상 경로
# /Game/... = 메인 프로젝트 Content 폴더
# /PluginName/... = 플러그인 Content 폴더 (플러그인에 CanContainContent: true 필요)
# Destination path:
DESTINATION_PATH = "/InteractionSystem/Textures"

# ============================================================================
# 스크립트 로직 - 수정 불필요
# Script Logic - No modification needed
# ============================================================================

def import_texture_for_ui(source_path: str, destination_path: str) -> bool:
    """
    Import a texture with UI-optimized settings
    
    Args:
        source_path: Absolute path to the source image file
        destination_path: Destination path (plugin or game)
    
    Returns:
        True if successful, False otherwise
    """
    
    # Get filename without extension for asset name
    import os
    filename = os.path.splitext(os.path.basename(source_path))[0]
    
    # Build the full destination path (no /Game/ prefix for plugin content)
    full_destination = destination_path
    
    # Create asset import task
    task = unreal.AssetImportTask()
    task.filename = source_path
    task.destination_path = full_destination
    task.destination_name = filename
    task.replace_existing = True
    task.automated = True
    task.save = True
    
    # Import the texture
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    asset_tools.import_asset_tasks([task])
    
    # Check if import was successful
    asset_path = f"{full_destination}/{filename}"
    imported_texture = unreal.EditorAssetLibrary.load_asset(asset_path)
    
    if imported_texture is None:
        unreal.log_error(f"Failed to import: {source_path}")
        return False
    
    # Apply UI texture settings
    if isinstance(imported_texture, unreal.Texture2D):
        # Set compression to UserInterface2D for best UI quality
        imported_texture.set_editor_property("compression_settings", 
            unreal.TextureCompressionSettings.TC_EDITOR_ICON)
        
        # Enable sRGB
        imported_texture.set_editor_property("srgb", True)
        
        # Disable mipmaps for UI (optional, cleaner at all sizes)
        imported_texture.set_editor_property("mip_gen_settings",
            unreal.TextureMipGenSettings.TMGS_NO_MIPMAPS)
        
        # Save the modified texture
        unreal.EditorAssetLibrary.save_asset(asset_path)
        
        unreal.log(f"Successfully imported with UI settings: {asset_path}")
        return True
    else:
        unreal.log_warning(f"Imported but not a Texture2D: {asset_path}")
        return False


def main():
    """Main entry point"""
    
    unreal.log("=" * 60)
    unreal.log("UI Texture Import Script")
    unreal.log("=" * 60)
    
    success_count = 0
    fail_count = 0
    
    for file_path in FILES_TO_IMPORT:
        unreal.log(f"Importing: {file_path}")
        
        if import_texture_for_ui(file_path, DESTINATION_PATH):
            success_count += 1
        else:
            fail_count += 1
    
    unreal.log("=" * 60)
    unreal.log(f"Import completed: {success_count} success, {fail_count} failed")
    unreal.log("=" * 60)


# Run the script
if __name__ == "__main__":
    main()
