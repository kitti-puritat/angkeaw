<?php
/* This file is part of a copyrighted work; it is distributed with NO WARRANTY.
 * See the file COPYRIGHT.html for more details.
 */
 
$_Nav_menu = array();
$_Nav_unparented = array();
class Nav {
  # The given $url is an URL do not HTML-escape it!
  public static function node($path, $title, $url=NULL) {
    Nav::_node($path, $title, $url);
    Nav::_reparent();
  }
  public static function _node($path, $title, $url) {
    $parent =& Nav::_getParent($path);
    $parent[] = array('path'=>$path, 'title'=>$title, 'url'=>$url, 'children'=>array());
  }
 public static function display($activePath) {
    global $_Nav_menu;
    Nav::_display($activePath, $_Nav_menu);
  }
 public static function _display($activePath, $menu, $class='nav_main') {
    echo '<ul class="'.$class.'">';
    foreach ($menu as $m) {
      if ($m['path'] != $activePath) {
        $link = '<a href="'.H($m['url']).'">'.H($m['title']).'</a>';
      } else {
        $link = H($m['title']);
      }
      if (Nav::_pathWithin($activePath, $m['path'])) {
        echo '<li class="active">'.$link;
        Nav::_display($activePath, $m['children'], 'nav_sub');
        echo '</li>';
      } elseif ($m['url']) {
        echo '<li>'.$link.'</li>';
      }
    }
    echo '</ul>';
  }
 public static function _pathWithin($sub, $path) {
    return ($sub == $path) or ($path.'/' == substr($sub, 0, strlen($path)+1));
  }
 public static function &_getParent($path) {
    global $_Nav_menu;
    if (preg_match('/^(.*)\/([^\/]*)?$/', $path, $m)) {
      $path = $m[1];
    } else {
      $path = "";
    }
    return Nav::_getParent_real($path, $_Nav_menu);
  }
  public static function &_getParent_real($path, &$menu, $curpath="") {
    global $_Nav_unparented;
    if ($path == $curpath) {
      return $menu;
    }
    # Not using foreach because it assigns copies, not references.
    for ($i=0; $i < count($menu); $i++) {
      if (Nav::_pathWithin($path, $menu[$i]['path'])) {
        return Nav::_getParent_real($path, $menu[$i]['children'], $menu[$i]['path']);
      }
    }
    return $_Nav_unparented;
  }
 public static function _reparent() {
    global $_Nav_unparented;
    $nodes = $_Nav_unparented;
    $_Nav_unparented = array();
    foreach ($nodes as $n) {
      Nav::_node($n['path'], $n['title'], $n['url']);
    }
  }
}

?>
