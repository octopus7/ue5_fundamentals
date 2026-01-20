"""
Apply UI Texture Settings - Context Menu Action
선택한 텍스처에 UI 설정 일괄 적용

Usage / 사용법:
1. Content Browser에서 텍스처 선택
2. Tools → Execute Python Script → ApplyUITextureSettings.py
"""

import unreal

def apply_ui_settings_to_selected():
    """Apply UI texture settings to all selected textures in Content Browser"""
    
    # Get selected assets
    editor_utility = unreal.EditorUtilityLibrary()
    selected_assets = editor_utility.get_selected_assets()
    
    if not selected_assets:
        unreal.log_warning("No assets selected. Please select textures in Content Browser.")
        return
    
    success_count = 0
    skip_count = 0
    
    for asset in selected_assets:
        if isinstance(asset, unreal.Texture2D):
            # Apply UI settings
            asset.set_editor_property("compression_settings", 
                unreal.TextureCompressionSettings.TC_EDITOR_ICON)
            asset.set_editor_property("srgb", True)
            asset.set_editor_property("mip_gen_settings",
                unreal.TextureMipGenSettings.TMGS_NO_MIPMAPS)
            
            # Get asset path for saving
            asset_path = asset.get_path_name()
            package_path = asset_path.rsplit('.', 1)[0]
            unreal.EditorAssetLibrary.save_asset(package_path)
            
            unreal.log(f"Applied UI settings to: {asset.get_name()}")
            success_count += 1
        else:
            unreal.log_warning(f"Skipping non-texture asset: {asset.get_name()}")
            skip_count += 1
    
    unreal.log("=" * 50)
    unreal.log(f"Completed: {success_count} textures updated, {skip_count} skipped")
    unreal.log("=" * 50)

# Run
apply_ui_settings_to_selected()
