<?php
/**
 * COmanage Registry CO Service Token Model
 *
 * Copyright (C) 2017 SURFnet BV
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software distributed under
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 *
 * @copyright     Copyright (C) 2017 SURFnet BV
 * @link          http://www.internet2.edu/comanage COmanage Project
 * @package       registry-plugin
 * @since         COmanage Registry v1.1.0
 * @license       Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
 * @version       $Id$
 */

class CoServiceToken extends AppModel {
  // Required by COmanage Plugins
  // To enable this plugin (even though it doesn't do anything), change the type to 'enroller'
  public $cmPluginType = "other";
  
  // Document foreign keys
  public $cmPluginHasMany = array();
  
  /**
   * Expose menu items.
   * 
   * @since COmanage Registry v0.9.4
   * @return Array with menu location type as key and array of labels, controllers, actions as values.
   */
  
  public function cmPluginMenus() {
    return array();
  }
}
