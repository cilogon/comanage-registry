<!--
/**
 * COmanage Registry CO API Provisioning Target Fields
 *
 * Portions licensed to the University Corporation for Advanced Internet
 * Development, Inc. ("UCAID") under one or more contributor license agreements.
 * See the NOTICE file distributed with this work for additional information
 * regarding copyright ownership.
 *
 * UCAID licenses this file to you under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * @link          http://www.internet2.edu/comanage COmanage Project
 * @package       registry-plugin
 * @since         COmanage Registry v4.0.0
 * @license       Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
 */
-->
<?php
  // Determine if fields are editable
  $e = false;
  
  if(($this->action == "add" && $permissions['add']) || ($this->action == "edit" && $permissions['edit']))
    $e = true;
    
  // We shouldn't get here if we don't have at least read permission, but check just in case
  
  if(!$e && !$permissions['view'])
    return false;
  
  // Add breadcrumbs
  print $this->element("coCrumb");

  $args = array();
  $args['plugin'] = null;
  $args['controller'] = 'co_provisioning_targets';
  $args['action'] = 'index';
  $args['co'] = $cur_co['Co']['id'];
  $this->Html->addCrumb(_txt('ct.co_provisioning_targets.pl'), $args);

  $args = array();
  $args['plugin'] = null;
  $args['controller'] = 'co_provisioning_targets';
  $args['action'] = 'edit';
  $args[] = $vv_ptid;
  $this->Html->addCrumb($co_api_provisioner_targets[0]['CoProvisioningTarget']['description'], $args);

  $crumbTxt = _txt('op.config');
  $this->Html->addCrumb($crumbTxt);
  
  print $this->Form->hidden('co_id', array('default' => $cur_co['Co']['id'])) . "\n";
  print $this->Form->hidden('co_provisioning_target_id', array('default' => $vv_ptid)) . "\n";
?>
<?php if(empty($vv_servers)): ?>
<div class="co-info-topbox">
  <i class="material-icons">info</i>
  <?php print _txt('er.server.none', array(_txt('en.server', null, ServerEnum::HttpServer))); ?>
</div>
<?php else: // vv_servers ?>
<ul id="<?php print $this->action; ?>_co_api_provisioner_target" class="fields form-list form-list-admin">
  <li>
    <div class="field-name">
      <?php print ($e ? $this->Form->label('server_id', _txt('fd.server')) : _txt('fd.server')); ?>
      <span class="required">*</span>
    </div>
    <div class="field-info">
      <?php
        $attrs = array();
        $attrs['value'] = (isset($co_api_provisioner_targets[0]['CoApiProvisionerTarget']['server_id']) 
                           ? $co_api_provisioner_targets[0]['CoApiProvisionerTarget']['server_id']
                           : null);
        // Since required=true, this will render blank by default but not allow save until a server is selected
        $attrs['empty'] = true;

        if($e) {
          print $this->Form->select('server_id',
                                    $vv_servers,
                                    $attrs);

          if($this->Form->isFieldError('server_id')) {
            print $this->Form->error('server_id');
          }
        } else {
          if(!empty($co_api_provisioner_targets[0]['CoApiProvisionerTarget']['server_id'])) {
            print filter_var($vv_servers[ $co_api_provisioner_targets[0]['CoApiProvisionerTarget']['server_id'] ], FILTER_SANITIZE_SPECIAL_CHARS);
          }
        }
      ?>
    </div>
  </li>
  <li>
    <div class="field-name">
      <div class="field-title">
        <?php print _txt('pl.apiprovisioner.mode'); ?>
        <span class="required">*</span>
      </div>
    </div>
    <div class="field-info">
      <?php
        global $cm_lang, $cm_texts;
        
        $attrs = array();
        $attrs['value'] = (isset($co_api_provisioner_targets[0]['CoApiProvisionerTarget']['mode'])
                           ? $co_api_provisioner_targets[0]['CoApiProvisionerTarget']['mode']
                           : "");
        $attrs['empty'] = true;

        if($e) {
          print $this->Form->select('mode',
                                    $cm_texts[ $cm_lang ]['pl.apiprovisioner.en.sync'],
                                    $attrs);

          if($this->Form->isFieldError('mode')) {
            print $this->Form->error('mode');
          }
        } else {
          print _txt('pl.apiprovisioner.en.sync', null, filter_var($co_api_provisioner_targets[0]['CoApiProvisionerTarget']['mode'],FILTER_SANITIZE_SPECIAL_CHARS));
        }
      ?>
    </div>
  </li>
  <li>
    <div class="field-name">
      <div class="field-title">
        <?php print _txt('pl.apiprovisioner.identifier_type'); ?>
        <span class="required">*</span>
      </div>
      <div class="field-desc"><?php print _txt('pl.apiprovisioner.identifier_type.desc'); ?></div>
    </div>
    <div class="field-info">
      <?php
        $attrs = array();
        $attrs['value'] = (isset($co_api_provisioner_targets[0]['CoApiProvisionerTarget']['identifier_type'])
                           ? $co_api_provisioner_targets[0]['CoApiProvisionerTarget']['identifier_type']
                           : "");
        $attrs['empty'] = true;

        if($e) {
          print $this->Form->select('identifier_type',
                                    $vv_identifiers_types,
                                    $attrs);

          if($this->Form->isFieldError('identifier_type')) {
            print $this->Form->error('identifier_type');
          }
        } else {
          print _txt('en.identifier.type', null, filter_var($co_api_provisioner_targets[0]['CoApiProvisionerTarget']['identifier_type'],FILTER_SANITIZE_SPECIAL_CHARS));
        }
      ?>
    </div>
  </li>
  <li>
    <div class="field-name">
      <div class="field-title">
        <?php print _txt('pl.apiprovisioner.include_attributes'); ?>
      </div>
      <div class="field-desc"><?php print _txt('pl.apiprovisioner.include_attributes.desc'); ?></div>
    </div>
    <div class="field-info">
      <?php print ($e ? $this->Form->input('include_attributes') : (isset($co_api_provisioner_targets[0]['CoApiProvisionerTarget']['include_attributes']) ? _txt('fd.yes') : _txt('fd.no'))); ?>
    </div>
  </li>
  <?php if($e): ?>
  <li class="fields-submit">
    <div class="field-name">
      <span class="required"><?php print _txt('fd.req'); ?></span>
    </div>
    <div class="field-info">
      <?php print $this->Form->submit($submit_label); ?>
    </div>
  </li>
  <?php endif; ?>
</ul>
<?php endif; // vv_servers